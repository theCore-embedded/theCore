#ifndef PLATFORM_I2C_BUS_HPP_
#define PLATFORM_I2C_BUS_HPP_

#include <stm32_device.hpp>
#include <common/irq.hpp>

#include <common/i2c.hpp>

#include <sys/types.h>
#include <ecl/utils.hpp>

namespace ecl
{

enum class i2c_mode
{
    POLL,
    IRQ
};

template< i2c_device        dev,
          i2c_mode          mode,
          uint32_t          clock_speed,
          uint16_t          operation_mode,
          uint16_t          duty_cycle,
          uint16_t          own_address,
          uint16_t          ack,
          uint16_t          acknowledged_address>
struct i2c_config
{
    static constexpr I2C_InitTypeDef m_init_obj = {
        clock_speed,
        operation_mode,
        duty_cycle,
        own_address,
        ack,
        acknowledged_address
    };

    static constexpr i2c_device m_dev = dev;
    static constexpr i2c_mode m_mode = mode;
};

template<class i2c_config>
class i2c_bus
{
public:
    // Convenient type aliases.
    using channel       = bus_channel;
    using event         = bus_event;
    using handler_fn    = bus_handler;

    // Static use only
    i2c_bus() = delete;
    ~i2c_bus() = delete;

    //!
    //! \brief Lazy initialization.
    //! \return Status of operation.
    //!
    static ecl::err init();

    //!
    //! \brief Sets rx buffer with given size.
    //! \param[in,out]  rx      Buffer to write data to. Optional.
    //! \param[in]      size    Size
    //!
    static void set_rx(uint8_t *rx, size_t size);

    //!
    //! \brief Sets tx buffer made-up from sequence of similar bytes.
    //! \param[in] size         Size of sequence
    //! \param[in] fill_byte    Byte to fill a sequence. Optional.
    //!
    static void set_tx(size_t size, uint8_t fill_byte = 0xff);

    //!
    //! \brief Sets tx buffer with given size.
    //! \param[in] tx   Buffer to transmit. Optional.
    //! \param[in] size Buffer size.
    //!
    static void set_tx(const uint8_t *tx, size_t size);

    //!
    //! \brief Sets event handler.
    //! Handler will be used by the bus, until reset_handler() will be called.
    //! \param[in] handler Handler itself.
    //!
    static void set_handler(const handler_fn &handler);

    //!
    //! \brief Reset xfer buffers.
    //! Buffers that were set by \sa set_tx() and \sa set_rx()
    //! will be no longer used after this call.
    //!
    static void reset_buffers();

    //!
    //! \brief Resets previously set handler.
    //!
    static void reset_handler();

    //!
    //! \brief Executes xfer, using buffers previously set.
    //! When it will be done, handler will be invoked.
    //! \return Status of operation.
    //!
    static ecl::err do_xfer();

    //!
    //! \brief Sets slave address.
    //! This address is used during communication.
    //! In case there is only one device on the bus,
    //! this function may be called only once,
    //! before first xfer().
    static void set_slave_addr(uint16_t addr);

private:
    static constexpr auto pick_rcc();
    static constexpr auto pick_rcc_fn();
    static constexpr auto pick_i2c();
    static constexpr auto pick_ev_irqn();
    static constexpr auto pick_er_irqn();

    //! Communication direction
    // TODO Add slave support
    static enum direction
    {
        MASTER_TX,
        MASTER_RX,
    } m_direction;

    // Device status flags

    //! Bit set if device initialized.
    static constexpr uint8_t m_inited     = 0x1;

    // Device status methods
    static inline bool inited()           { return (m_status & m_inited)  != 0; }

    static inline void set_inited()       { m_status |= m_inited; }
    static inline void clear_inited()     { m_status &= ~(m_inited); }

    //! Setup xfer in IRQ mode
    static ecl::err i2c_setup_xfer_irq();

    //! Operations for POLL mode
    static ecl::err i2c_transmit_poll();
    static ecl::err i2c_receive_poll();

    //! Handles IRQ events from a bus.
    static void irq_ev_handler();
    //! Handles IRQ events (error) from a bus.
    static void irq_er_handler();

    //! Helper functions for bytes receive/transmit
    static void receive_bytes(size_t count);
    static void send_bytes(size_t count);

    static const uint8_t   *m_tx;           //! Transmit buffer.
    static size_t          m_tx_size;       //! TX buffer size.
    static size_t          m_tx_left;       //! Left to send in TX buffer.
    static uint8_t         *m_rx;           //! Receive buffer.
    static size_t          m_rx_size;       //! RX buffer size.
    static size_t          m_rx_left;       //! Left to receive in RX buffer.
    static uint8_t         m_status;        //! Tracks device status.

    static uint16_t        m_own_addr;      //! device address in slave mode
    static uint16_t        m_slave_addr;    //! address of slave to communicate

    //! Private handler storage.
    static safe_storage<handler_fn> m_handler_storage;

    //! Gets event handler
    static constexpr auto &get_handler() { return reinterpret_cast<handler_fn&>(m_handler_storage); }
};

template<class i2c_config>
typename i2c_bus<i2c_config>::direction i2c_bus<i2c_config>::m_direction;

template<class i2c_config>
const uint8_t *i2c_bus<i2c_config>::m_tx;

template<class i2c_config>
uint8_t *i2c_bus<i2c_config>::m_rx;

template<class i2c_config>
size_t i2c_bus<i2c_config>::m_tx_size;

template<class i2c_config>
size_t i2c_bus<i2c_config>::m_tx_left;

template<class i2c_config>
size_t i2c_bus<i2c_config>::m_rx_size;

template<class i2c_config>
size_t i2c_bus<i2c_config>::m_rx_left;

template<class i2c_config>
uint8_t i2c_bus<i2c_config>::m_status;

template<class i2c_config>
uint16_t i2c_bus<i2c_config>::m_own_addr;

template<class i2c_config>
uint16_t i2c_bus<i2c_config>::m_slave_addr;

template<class i2c_config>
safe_storage<typename i2c_bus<i2c_config>::handler_fn> i2c_bus<i2c_config>::m_handler_storage;

template <class i2c_config>
ecl::err i2c_bus<i2c_config>::init()
{
    if (inited()) {
        return ecl::err::ok;
    }

    constexpr auto i2c = pick_i2c();
    constexpr auto rcc_periph = pick_rcc();
    constexpr auto rcc_fn = pick_rcc_fn();

    rcc_fn(rcc_periph, ENABLE);

    constexpr auto init_const_obj = i2c_config::m_init_obj;
    auto init_obj = init_const_obj;
    I2C_Init(i2c, &init_obj);

    I2C_Cmd(i2c, ENABLE);

    if (i2c_config::m_mode == i2c_mode::IRQ) {
        constexpr auto irqn_ev = pick_ev_irqn();
        constexpr auto irqn_er = pick_er_irqn();

        auto lambda_ev = []() {
            irq_ev_handler();
        };

        auto lambda_er = []() {
            irq_er_handler();
        };

        irq::subscribe(irqn_ev, lambda_ev);
        irq::subscribe(irqn_er, lambda_er);
    }

    set_inited();

    return ecl::err::ok;
}

template<class i2c_config>
void i2c_bus<i2c_config>::set_rx(uint8_t *rx, size_t size)
{
    if (!inited()) {
        return;
    }

    m_rx = rx;
    m_rx_size = size;
    m_rx_left = m_rx_size;
}

template<class i2c_config>
void i2c_bus<i2c_config>::set_tx(const uint8_t *tx, size_t size)
{
    if (!inited()) {
        return;
    }

    m_tx = tx;
    m_tx_size = size;
    m_tx_left = m_tx_size;
}

template<class i2c_config>
void i2c_bus<i2c_config>::set_tx(size_t size, uint8_t fill_byte)
{
    if (!inited()) {
        return;
    }

    // TODO implement functionality for this method
    (void) size;
    (void) fill_byte;
}

template<class i2c_config>
ecl::err i2c_bus<i2c_config>::do_xfer()
{
    if (!inited()) {
        return ecl::err::generic;
    }

    if (i2c_config::m_mode == i2c_mode::POLL) {
        if (m_tx) {
            i2c_transmit_poll();
            get_handler()(channel::tx, event::tc, m_tx_size);
        }

        if (m_rx) {
            i2c_receive_poll();
            get_handler()(channel::rx, event::tc, m_rx_size);
        }

        get_handler()(channel::meta, event::tc, m_rx_size | m_tx_size);
    } else if (i2c_config::m_mode == i2c_mode::IRQ) {
        // There are common interrupts for tx and rx.
        // If no tx is required then rx will be done,
        // otherwise rx will be set in ISR, when tx
        // will be finished
        if (m_tx) {
            m_direction = MASTER_TX;
        } else if (m_rx) {
            m_direction = MASTER_RX;
        }

        constexpr auto irqn = pick_ev_irqn();
        i2c_setup_xfer_irq();
        irq::unmask(irqn);
    }

    return ecl::err::ok;
}

template<class i2c_config>
void i2c_bus<i2c_config>::reset_buffers()
{
    m_tx = nullptr;
    m_rx = nullptr;
    m_tx_left = m_tx_size = 0;
    m_rx_left = m_rx_size = 0;
}

template<class i2c_config>
void i2c_bus<i2c_config>::set_handler(const handler_fn &handler)
{
    get_handler() = handler;
}

template<class i2c_config>
ecl::err i2c_bus<i2c_config>::i2c_setup_xfer_irq()
{
    constexpr auto i2c = pick_i2c();

    while (I2C_GetFlagStatus(i2c, I2C_FLAG_BUSY) == SET);

    // TODO Add error handling and enable error irq
    I2C_ITConfig(i2c, I2C_IT_EVT | I2C_IT_BUF, ENABLE);

    I2C_GenerateSTART(i2c, ENABLE);

    return ecl::err::ok;
}

template<class i2c_config>
ecl::err i2c_bus<i2c_config>::i2c_transmit_poll()
{
    constexpr auto i2c = pick_i2c();

    while (I2C_GetFlagStatus(i2c, I2C_FLAG_BUSY) == SET);

    I2C_GenerateSTART(i2c, ENABLE);
    while (I2C_GetFlagStatus(i2c, I2C_FLAG_SB) == RESET);

    I2C_Send7bitAddress(i2c, m_slave_addr, I2C_Direction_Transmitter);
    while (I2C_GetFlagStatus(i2c, I2C_FLAG_ADDR) == RESET);

    /* clearing address sent bit according to RM */
    i2c->SR1; i2c->SR2;

    for (size_t i = 0; i < m_tx_size; i++) {
        while (I2C_GetFlagStatus(i2c, I2C_FLAG_TXE) == RESET);
        I2C_SendData(i2c, m_tx[i]);
        while (I2C_GetFlagStatus(i2c, I2C_FLAG_BTF) == RESET);
    }

    I2C_GenerateSTOP(i2c, ENABLE);

    return ecl::err::ok;
}

template<class i2c_config>
ecl::err i2c_bus<i2c_config>::i2c_receive_poll()
{
    constexpr auto i2c = pick_i2c();

    while (I2C_GetFlagStatus(i2c, I2C_FLAG_BUSY) == SET);
    I2C_AcknowledgeConfig(i2c, ENABLE);

    I2C_GenerateSTART(i2c, ENABLE);
    while (I2C_GetFlagStatus(i2c, I2C_FLAG_SB) == RESET);

    I2C_Send7bitAddress(i2c, m_slave_addr, I2C_Direction_Receiver);
    while (I2C_GetFlagStatus(i2c, I2C_FLAG_ADDR) == RESET);

    // single byte reception is a special case, see RM
    if (m_rx_size == 1) {
        I2C_AcknowledgeConfig(i2c, DISABLE);

        // clearing address sent bit according to RM
        i2c->SR1; i2c->SR2;

        while (I2C_GetFlagStatus(i2c, I2C_FLAG_RXNE) == RESET);
        receive_bytes(1);

        I2C_GenerateSTOP(i2c, ENABLE);

    } else if (m_rx_size == 2) {
        // two bytes reception is a special case, see RM
        I2C_AcknowledgeConfig(i2c, DISABLE);
        I2C_NACKPositionConfig(i2c, I2C_NACKPosition_Next);

        i2c->SR1; i2c->SR2;

        while (I2C_GetFlagStatus(i2c, I2C_FLAG_BTF) == RESET);
        I2C_GenerateSTOP(i2c, ENABLE);

        // read 1 and 2
        receive_bytes(2);
    } else if (m_rx_size > 2){
        i2c->SR1; i2c->SR2;

        // for N > 2 byte reception from 0 to N-2 bytes
        while (m_rx_left > 3) {
            while (I2C_GetFlagStatus(i2c, I2C_FLAG_RXNE) == RESET);
            receive_bytes(1);
        }
        // for N > 2 byte reception, from N-2 data reception
        while (I2C_GetFlagStatus(i2c, I2C_FLAG_BTF) == RESET);

        // set ACK low
        I2C_AcknowledgeConfig(i2c, DISABLE);
        // read data N-2
        receive_bytes(1);

        // wait until BTF = 1
        // (data N-1 in DR, data N in shift register,
        // SCL stretched low until a data N-1 is read)
        while (I2C_GetFlagStatus(i2c, I2C_FLAG_BTF) == RESET);

        // set STOP high
        I2C_GenerateSTOP(i2c, ENABLE);

        // read data N-1 and N
        receive_bytes(2);
    }

    return ecl::err::ok;
}

template<class i2c_config>
constexpr auto i2c_bus<i2c_config>::pick_ev_irqn()
{
    constexpr auto i2c = pick_i2c();

    if (i2c == I2C1) {
        return I2C1_EV_IRQn;
    } else if (i2c == I2C2) {
        return I2C2_EV_IRQn;
#ifdef I2C3
    } else if (i2c == I2C3) {
        return I2C3_EV_IRQn;
#endif
    }
}

template<class i2c_config>
constexpr auto i2c_bus<i2c_config>::pick_er_irqn()
{
    constexpr auto i2c = pick_i2c();

    if (i2c == I2C1) {
        return I2C1_ER_IRQn;
    } else if (i2c == I2C2) {
        return I2C2_ER_IRQn;
#ifdef I2C3
    } else if (i2c == I2C3) {
        return I2C3_ER_IRQn;
#endif
    }
}

template<class i2c_config>
constexpr auto i2c_bus<i2c_config>::pick_i2c()
{
    switch (i2c_config::m_dev) {
    case i2c_device::bus1:
        return I2C1;
    case i2c_device::bus2:
        return I2C2;
#ifdef I2C3
    case i2c_device::bus3:
        return I2C3;
#endif
    default:
        return static_cast< decltype(I2C1) >(nullptr);
    }
}

template<class i2c_config>
constexpr auto i2c_bus<i2c_config>::pick_rcc()
{
    switch (i2c_config::m_dev) {
    case i2c_device::bus1:
        return RCC_APB1Periph_I2C1;
    case i2c_device::bus2:
        return RCC_APB1Periph_I2C2;
#ifdef I2C3
    case i2c_device::bus3:
        return RCC_APB1Periph_I2C3;
#endif
    default:
        return static_cast< decltype(RCC_APB1Periph_I2C1) >(-1);
    }
}

template<class i2c_config>
constexpr auto i2c_bus<i2c_config>::pick_rcc_fn()
{
    switch (i2c_config::m_dev) {
    case i2c_device::bus1:
    case i2c_device::bus2:
    case i2c_device::bus3:
        return RCC_APB1PeriphClockCmd;
    default:
        return static_cast< decltype(&RCC_APB1PeriphClockCmd) >(nullptr);
    }
}

template<class i2c_config>
void i2c_bus<i2c_config>::receive_bytes(size_t count)
{
    constexpr auto i2c = pick_i2c();

    if (!m_rx || !m_rx_left) {
        return;
    }

    for (size_t i = 0; i < count && m_rx_left; i++) {
        m_rx[m_rx_size - m_rx_left] = I2C_ReceiveData(i2c);
        m_rx_left--;
    }
}

template<class i2c_config>
void i2c_bus<i2c_config>::send_bytes(size_t count)
{
    constexpr auto i2c = pick_i2c();

    if (!m_tx || !m_tx_left) {
        return;
    }

    for (size_t i = 0; i < count && m_tx_left; i++) {
        I2C_SendData(i2c, m_tx[m_tx_size - m_tx_left]);
        m_tx_left--;
    }
}

template<class i2c_config>
void i2c_bus<i2c_config>::irq_ev_handler()
{
    constexpr auto irqn  = pick_ev_irqn();
    constexpr auto i2c = pick_i2c();

    irq::clear(irqn);

    // master mode selected
    if (i2c->SR1 & I2C_FLAG_SB) {
        I2C_Send7bitAddress(i2c,
                m_slave_addr,
                m_direction == MASTER_RX ?
                        I2C_Direction_Receiver:
                        I2C_Direction_Transmitter);

        // ? goto will be good here, but it is some sort of bad style in C++
        irq::unmask(irqn);
        return;
    }

    if (i2c->SR1 & I2C_FLAG_ADDR) {
        // this also clears ADDR bit
        if (i2c->SR2 & (I2C_FLAG_TRA >> 16)) {
            // master transmit mode
            send_bytes(1);
        } else {
            // single byte reception is special, see RM
            // it will be an additional interrupt when byte will be received
            if (m_rx_size == 1) {
                I2C_AcknowledgeConfig(i2c, DISABLE);
            } else if (m_rx_size == 2) {
                // two bytes reception is a special case, see RM
                I2C_AcknowledgeConfig(i2c, DISABLE);
                I2C_NACKPositionConfig(i2c, I2C_NACKPosition_Next);
            } else {
                I2C_AcknowledgeConfig(i2c, ENABLE);
            }
        }

        irq::unmask(irqn);
        return;
    }

    // byte was transmitted
    if (i2c->SR1 & I2C_FLAG_TXE) {
        if (m_tx_left > 0) {
            send_bytes(1);
        } else {
            // all bytes were transmitted
            I2C_GenerateSTOP(i2c, ENABLE);
            get_handler()(channel::tx, event::tc, 0);

            // tx is done here, set up rx
            if (m_rx) {
                m_direction = MASTER_RX;
                i2c_setup_xfer_irq();
            } else {
                get_handler()(channel::tx, event::tc, m_tx_size);
                // transfer is complete
                get_handler()(channel::meta, event::tc, m_tx_size);
                I2C_ITConfig(i2c, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
                return;
            }
        }
    }

    // byte was received
    if (i2c->SR1 & I2C_FLAG_RXNE) {
        if (m_rx_left == 1 && m_rx_size == 1) { // single byte case
            receive_bytes(1);
            I2C_GenerateSTOP(i2c, ENABLE);
        } else if (m_rx_size == 2) { // two byte case
            // here we need to check BTF, according to RM
            // if it is not set - it will interrupt
            if (i2c->SR1 & I2C_FLAG_BTF) {
                I2C_GenerateSTOP(i2c, ENABLE);
                // N-1 and N
                receive_bytes(2);
            } else {
                // disable TXE and RXNE interrupt while waiting for BTF
                I2C_ITConfig(i2c, I2C_IT_BUF, DISABLE);
            }
        } else if (m_rx_size > 2) {
            // see N > 2 bytes reception in RM
            if (m_rx_left == 3) { // reception from N-2 to N bytes
                if (i2c->SR1 & I2C_FLAG_BTF) {
                    I2C_AcknowledgeConfig(i2c, DISABLE);
                    // read data N-2
                    receive_bytes(1);
                } else {
                    // disable TXE and RXNE interrupt while waiting for BTF
                    I2C_ITConfig(i2c, I2C_IT_BUF, DISABLE);
                }
            } else if (m_rx_left == 2) {
                // check on BTF according to RM
                if (i2c->SR1 & I2C_FLAG_BTF) {
                    I2C_GenerateSTOP(i2c, ENABLE);
                    // read data N-1 and N
                    receive_bytes(2);
                } else {
                    // disable TXE and RXNE interrupt while waiting for BTF
                    I2C_ITConfig(i2c, I2C_IT_BUF, DISABLE);
                }
            } else if (m_rx_left > 3) { // reception from 0 to N-2 bytes
                receive_bytes(1);
            }
        }

        // rx is done (as well as tx)
        if (m_rx_left == 0) {
            get_handler()(channel::rx, event::tc, m_rx_size);

            // rx always last, so transfer is complete
            get_handler()(channel::meta, event::tc, m_rx_size);
            I2C_ITConfig(i2c, I2C_IT_EVT | I2C_IT_BUF, DISABLE);

            return;
        }
    }

    irq::unmask(irqn);
}

template<class i2c_config>
void i2c_bus<i2c_config>::irq_er_handler()
{
    // TODO add error handling
}

template<class i2c_config>
void i2c_bus<i2c_config>::set_slave_addr(uint16_t addr)
{
    m_slave_addr = addr;
}

}
#endif /* PLATFORM_I2C_BUS_HPP_ */
