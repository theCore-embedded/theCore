#ifndef PLATFORM_I2C_BUS_HPP_
#define PLATFORM_I2C_BUS_HPP_

#include <stm32f4xx_i2c.h>
#include <stm32f4xx_rcc.h>

#include <platform/irq_manager.hpp>

#include <common/i2c.hpp>

#include <sys/types.h>

namespace ecl {

enum class i2c_mode {
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
            .I2C_ClockSpeed = clock_speed,
            .I2C_Mode = operation_mode,
            .I2C_DutyCycle = duty_cycle,
            .I2C_OwnAddress1 = own_address,
            .I2C_Ack = ack,
            .I2C_AcknowledgedAddress = acknowledged_address
    };

    static constexpr i2c_device m_dev = dev;
    static constexpr i2c_mode m_mode = mode;
};

template <class i2c_config>
class i2c_bus
{
public:
    // Convenient type aliases.
    using channel       = bus_channel;
    using event         = bus_event;
    using handler_fn    = bus_handler;

    //!
    //! \brief Constructs a bus.
    //!
    i2c_bus();

    //!
    //! \brief Destructs a bus.
    ~i2c_bus();

    //!
    //! \brief Lazy initialization.
    //! \return Status of operation.
    //!
    ecl::err init();

    //!
    //! \brief Sets rx buffer with given size.
    //! \param[in,out]  rx      Buffer to write data to. Optional.
    //! \param[in]      size    Size
    //!
    void set_rx(uint8_t *rx, size_t size);

    //!
    //! \brief Sets rx buffer made-up from sequence of similar bytes.
    //! \param[in] size         Size of sequence
    //! \param[in] fill_byte    Byte to fill a sequence. Optional.
    //!
    void set_tx(size_t size, uint8_t fill_byte = 0xff);

    //!
    //! \brief Sets tx buffer with given size.
    //! \param[in] tx   Buffer to transmit. Optional.
    //! \param[in] size Buffer size.
    //!
    void set_tx(const uint8_t *tx, size_t size);

    //!
    //! \brief Sets event handler.
    //! Handler will be used by the bus, until reset_handler() will be called.
    //! \param[in] handler Handler itself.
    //!
    void set_handler(const handler_fn &handler);

    //!
    //! \brief Reset xfer buffers.
    //! Buffers that were set by \sa set_tx() and \sa set_rx()
    //! will be no longer used after this call.
    //!
    void reset_buffers();

    //!
    //! \brief Resets previously set handler.
    //!
    void reset_handler();

    //!
    //! \brief Executes xfer, using buffers previously set.
    //! When it will be done, handler will be invoked.
    //! \return Status of operation.
    //!
    ecl::err do_xfer();

    //!
    //! \brief Sets slave address.
    //! This address is used during communication.
    //! In case there is only one device on the bus,
    //! this function may be called only once,
    //! before first xfer().
    void set_slave_addr(uint16_t addr);

private:
    static constexpr auto pick_rcc();
    static constexpr auto pick_rcc_fn();
    static constexpr auto pick_i2c();
    static constexpr auto pick_ev_irqn();
    static constexpr auto pick_er_irqn();

    //! Communication direction
    // TODO Add slave support
    enum direction {
        MASTER_TX,
        MASTER_RX,
    } m_direction;

    // Device status flags

    //! Bit set if device initialized.
    static constexpr uint8_t m_inited     = 0x1;

    // Device status methods
    inline bool inited() const     { return (m_status & m_inited)  != 0; }

    inline void set_inited()       { m_status |= m_inited; }
    inline void clear_inited()     { m_status &= ~(m_inited); }

    //! Setup xfer in IRQ mode
    ecl::err i2c_setup_xfer_irq();

    //! Operations for POLL mode
    ecl::err i2c_transmit_poll();
    ecl::err i2c_receive_poll();

    //! Handles IRQ events from a bus.
    void irq_ev_handler();
    //! Handles IRQ events (error) from a bus.
    void irq_er_handler();

    //! Helper functions for byte receive/transmit
    void receive_byte(size_t count);
    void send_byte(size_t count);

    handler_fn      m_event_handler; //! Handler passed via set_handler().
    const uint8_t   *m_tx;           //! Transmit buffer.
    size_t          m_tx_size;       //! TX buffer size.
    size_t          m_tx_left;       //! Left to send in TX buffer.
    uint8_t         *m_rx;           //! Receive buffer.
    size_t          m_rx_size;       //! RX buffer size.
    size_t          m_rx_left;       //! Left to receive in RX buffer.
    uint8_t         m_status;        //! Tracks device status.

    uint16_t        m_own_addr;     //! device address in slave mode
    uint16_t        m_slave_addr;   //! address of slave to communicate
};

template< class i2c_config >
i2c_bus< i2c_config >::i2c_bus()
	:m_event_handler{}
	,m_tx{nullptr}
	,m_tx_size{0}
	,m_tx_left{0}
	,m_rx{nullptr}
	,m_rx_size{0}
	,m_rx_left{0}
	,m_status{0}
	,m_slave_addr{0}
{

}

template< class i2c_config >
i2c_bus< i2c_config >::~i2c_bus()
{

}

template < class i2c_config >
ecl::err i2c_bus <i2c_config>::init()
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

        auto lambda_ev = [this]() {
            this->irq_ev_handler();
        };

        auto lambda_er = [this]() {
            this->irq_er_handler();
        };

        IRQ_manager::subscribe(irqn_ev, lambda_ev);
        IRQ_manager::subscribe(irqn_er, lambda_er);
    }

    set_inited();

    return ecl::err::ok;
}

template< class i2c_config >
void i2c_bus< i2c_config >::set_rx(uint8_t *rx, size_t size)
{
    if (!inited()) {
        return;
    }

    m_rx = rx;
    m_rx_size = size;
    m_rx_left = m_rx_size;
}

template< class i2c_config >
void i2c_bus< i2c_config >::set_tx(const uint8_t *tx, size_t size)
{
    if (!inited()) {
        return;
    }

    m_tx = tx;
    m_tx_size = size;
    m_tx_left = m_tx_size;
}

template< class i2c_config >
void i2c_bus< i2c_config >::set_tx(size_t size, uint8_t fill_byte)
{
    if (!inited()) {
        return;
    }

    // TODO implement functionality for this method
    (void) size;
    (void) fill_byte;
}

template< class i2c_config >
ecl::err i2c_bus< i2c_config >::do_xfer()
{
    if (!inited()) {
        return ecl::err::generic;
    }

    if (i2c_config::m_mode == i2c_mode::POLL) {
        if (m_tx) {
            i2c_transmit_poll();
            m_event_handler(channel::tx, event::tc, m_tx_size);
        }

        if (m_rx) {
            i2c_receive_poll();
            m_event_handler(channel::rx, event::tc, m_rx_size);
        }

        m_event_handler(channel::meta, event::tc, m_rx_size | m_tx_size);
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
        IRQ_manager::unmask(irqn);
    }

    return ecl::err::ok;
}

template< class i2c_config >
void i2c_bus< i2c_config >::reset_buffers()
{
    m_tx = nullptr;
    m_rx = nullptr;
    m_tx_left = m_tx_size = 0;
    m_rx_left = m_rx_size = 0;
}

template< class i2c_config >
void i2c_bus< i2c_config >::set_handler(const handler_fn &handler)
{
    m_event_handler = handler;
}

template< class i2c_config >
ecl::err i2c_bus <i2c_config>::i2c_setup_xfer_irq()
{
    constexpr auto i2c = pick_i2c();

    while (I2C_GetFlagStatus(i2c, I2C_FLAG_BUSY) == SET);

    // TODO Add error handling and enable error irq
    I2C_ITConfig(i2c, I2C_IT_EVT | I2C_IT_BUF, ENABLE);

    I2C_GenerateSTART(i2c, ENABLE);

    return ecl::err::ok;
}

template< class i2c_config >
ecl::err i2c_bus <i2c_config>::i2c_transmit_poll()
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

template< class i2c_config >
ecl::err i2c_bus <i2c_config>::i2c_receive_poll()
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
        *m_rx = I2C_ReceiveData(i2c);

        I2C_GenerateSTOP(i2c, ENABLE);

    } else if (m_rx_size == 2) {
        // two bytes reception is a special case, see RM
        I2C_AcknowledgeConfig(i2c, DISABLE);
        I2C_NACKPositionConfig(i2c, I2C_NACKPosition_Next);

        i2c->SR1; i2c->SR2;

        while (I2C_GetFlagStatus(i2c, I2C_FLAG_BTF) == RESET);
        I2C_GenerateSTOP(i2c, ENABLE);

        // read 1 and 2
        m_rx[0] = I2C_ReceiveData(i2c);
        m_rx[1] = I2C_ReceiveData(i2c);
    } else if (m_rx_size > 2){
        i2c->SR1; i2c->SR2;

        // for N > 2 byte reception from 0 to N-2 bytes
        size_t i = 0;
        for (; i < m_rx_size - 3; i++) {
            while (I2C_GetFlagStatus(i2c, I2C_FLAG_RXNE) == RESET);
            m_rx[i] = I2C_ReceiveData(i2c);
        }

        // for N > 2 byte reception, from N-2 data reception
        while (I2C_GetFlagStatus(i2c, I2C_FLAG_BTF) == RESET);

        // set ACK low
        I2C_AcknowledgeConfig(i2c, DISABLE);
        // read data N-2
        m_rx[i++] = I2C_ReceiveData(i2c);

        // wait until BTF = 1
        // (data N-1 in DR, data N in shift register,
        // SCL stretched low until a data N-1 is read)
        while (I2C_GetFlagStatus(i2c, I2C_FLAG_BTF) == RESET);

        // set STOP high
        I2C_GenerateSTOP(i2c, ENABLE);

        // read data N-1 and N
        m_rx[i++] = I2C_ReceiveData(i2c);
        m_rx[i] = I2C_ReceiveData(i2c);
    }

    return ecl::err::ok;
}

template< class i2c_config >
constexpr auto i2c_bus< i2c_config >::pick_ev_irqn()
{
    constexpr auto i2c = pick_i2c();

    if (i2c == I2C1) {
        return I2C1_EV_IRQn;
    } else if (i2c == I2C2) {
        return I2C2_EV_IRQn;
    } else if (i2c == I2C3) {
        return I2C3_EV_IRQn;
    }
}

template< class i2c_config >
constexpr auto i2c_bus< i2c_config >::pick_er_irqn()
{
    constexpr auto i2c = pick_i2c();

    if (i2c == I2C1) {
        return I2C1_ER_IRQn;
    } else if (i2c == I2C2) {
        return I2C2_ER_IRQn;
    } else if (i2c == I2C3) {
        return I2C3_ER_IRQn;
    }
}

template< class i2c_config >
constexpr auto i2c_bus< i2c_config >::pick_i2c()
{
    switch (i2c_config::m_dev) {
    case i2c_device::bus_1:
        return I2C1;
    case i2c_device::bus_2:
        return I2C2;
    case i2c_device::bus_3:
        return I2C3;
    default:
        return static_cast< decltype(I2C1) >(nullptr);
    }
}

template< class i2c_config >
constexpr auto i2c_bus< i2c_config >::pick_rcc()
{
    switch (i2c_config::m_dev) {
    case i2c_device::bus_1:
        return RCC_APB1Periph_I2C1;
    case i2c_device::bus_2:
        return RCC_APB1Periph_I2C2;
    case i2c_device::bus_3:
        return RCC_APB1Periph_I2C3;
    default:
        return static_cast< decltype(RCC_APB1Periph_I2C1) >(-1);
    }
}

template< class i2c_config >
constexpr auto i2c_bus< i2c_config >::pick_rcc_fn()
{
    switch (i2c_config::m_dev) {
    case i2c_device::bus_1:
    case i2c_device::bus_2:
    case i2c_device::bus_3:
        return RCC_APB1PeriphClockCmd;
    default:
        return static_cast< decltype(&RCC_APB1PeriphClockCmd) >(nullptr);
    }
}

template< class i2c_config >
void i2c_bus< i2c_config >::receive_byte(size_t count)
{
    constexpr auto i2c = pick_i2c();

    if (!m_rx || !m_rx_left) {
        return;
    }

    for (size_t i = 0; i < count && m_rx_left; i++) {
        m_rx[m_rx_size - m_rx_left] = I2C_ReceiveData(i2c);
        m_rx_left--;
    }

    return;
}

template< class i2c_config >
void i2c_bus< i2c_config >::send_byte(size_t count)
{
    constexpr auto i2c = pick_i2c();

    if (!m_tx || !m_tx_left) {
        return;
    }

    for (size_t i = 0; i < count && m_tx_left; i++) {
        I2C_SendData(i2c, m_tx[m_tx_size - m_tx_left]);
        m_tx_left--;
    }

    return;
}

template< class i2c_config >
void i2c_bus< i2c_config >::irq_ev_handler()
{
    constexpr auto irqn  = pick_ev_irqn();
    constexpr auto i2c = pick_i2c();

    IRQ_manager::clear(irqn);

    // master mode selected
    if (i2c->SR1 & I2C_FLAG_SB) {
        I2C_Send7bitAddress(i2c,
                m_slave_addr,
                m_direction == MASTER_RX ?
                        I2C_Direction_Receiver:
                        I2C_Direction_Transmitter);

        // ? goto will be good here, but it is some sort of bad style in C++
        IRQ_manager::unmask(irqn);
        return;
    }

    if (i2c->SR1 & I2C_FLAG_ADDR) {
        // this also clears ADDR bit
        if (i2c->SR2 & (I2C_FLAG_TRA >> 16)) {
            // master transmit mode
            send_byte(1);
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

        IRQ_manager::unmask(irqn);
        return;
    }

    // byte was transmitted
    if (i2c->SR1 & I2C_FLAG_TXE) {
        if (m_tx_left > 0) {
            send_byte(1);
        } else {
            // all bytes were transmitted
            I2C_GenerateSTOP(i2c, ENABLE);
            m_event_handler(channel::tx, event::tc, 0);

            // tx is done here, set up rx
            if (m_rx) {
                m_direction = MASTER_RX;
                i2c_setup_xfer_irq();
            } else {
                m_event_handler(channel::tx, event::tc, m_tx_size);
                // transfer is complete
                m_event_handler(channel::meta, event::tc, m_tx_size);
                I2C_ITConfig(i2c, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
                return;
            }
        }
    }

    // byte was received
    if (i2c->SR1 & I2C_FLAG_RXNE) {
        if (m_rx_left == 1 && m_rx_size == 1) { // single byte case
            receive_byte(1);
            I2C_GenerateSTOP(i2c, ENABLE);
        } else if (m_rx_size == 2) { // two byte case
            // here we need to check BTF, according to RM
            // if it is not set - it will interrupt
            if (i2c->SR1 & I2C_FLAG_BTF) {
                I2C_GenerateSTOP(i2c, ENABLE);
                // N-1 and N
                receive_byte(2);
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
                    receive_byte(1);
                } else {
                    // disable TXE and RXNE interrupt while waiting for BTF
                    I2C_ITConfig(i2c, I2C_IT_BUF, DISABLE);
                }
            } else if (m_rx_left == 2) {
                // check on BTF according to RM
                if (i2c->SR1 & I2C_FLAG_BTF) {
                    I2C_GenerateSTOP(i2c, ENABLE);
                    // read data N-1 and N
                    receive_byte(2);
                } else {
                    // disable TXE and RXNE interrupt while waiting for BTF
                    I2C_ITConfig(i2c, I2C_IT_BUF, DISABLE);
                }
            } else if (m_rx_left > 3) { // reception from 0 to N-2 bytes
                receive_byte(1);
            }
        }

        // rx is done (as well as tx)
        if (m_rx_left == 0) {
            m_event_handler(channel::rx, event::tc, m_rx_size);

            // rx always last, so transfer is complete
            m_event_handler(channel::meta, event::tc, m_rx_size);
            I2C_ITConfig(i2c, I2C_IT_EVT | I2C_IT_BUF, DISABLE);

            return;
        }
    }

    IRQ_manager::unmask(irqn);

    return;
}

template< class i2c_config >
void i2c_bus< i2c_config >::irq_er_handler()
{
    // TODO add error handling
    return;
}

template< class i2c_config >
void i2c_bus< i2c_config >::set_slave_addr(uint16_t addr)
{
    m_slave_addr = addr;
}

}
#endif /* PLATFORM_I2C_BUS_HPP_ */
