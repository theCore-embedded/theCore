/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//!
//! \file
//! \brief STM32 USART driver
//!

#ifndef PLATFORM_USART_BUS_HPP_
#define PLATFORM_USART_BUS_HPP_

#include <common/bus.hpp>
#include <ecl/err.hpp>
#include <ecl/assert.h>
#include <common/irq.hpp>

#include <stm32_device.hpp>

#include <cstdint>
#include <unistd.h>

#include <functional>
#include <type_traits>

namespace ecl
{

//! Represents distinct peripheral devices
enum class usart_device
{
    dev1,
    dev2,
    dev3,
    dev4,
    dev5,
    dev6,
    dev7,
    dev8
};

//! Base template class for the usart configuration
//! \details User must create template specialization for required USART device.
//! Usart configuration class must contain following constexpr fields:
//! - uint32_t **baudrate** - Configures the USART communication baud rate
//! - uint16_t **word_len** - Specifies the number of data bits transmitted
//!                           or received. This member can be a value of
//!                           \ref USART_Word_Length (see STM32 SPL).
//! - uint16_t **stop_bit** - Specifies the number of stop bits transmitted.
//!                           This member can be a value of
//!                           \ref USART_Stop_Bits (see STM32 SPL).
//! - uint16_t **parity**   - Specifies the parity mode. This member can be
//!                           a value of \ref USART_Parity (see STM32 SPL).
//! - uint16_t **mode**     - Specifies wether the Receive or Transmit mode is
//!                           enabled or disabled. This member can be a value
//!                           of \ref USART_Modes (see STM32 SPL).
//! - uint16_t **hw_flow**  - Specifies wether the hardware flow control mode
//!                           is enabled or disabled. This member can be a value
//!                           of \ref USART_Hardware_Flow_Control (see STM32 SPL).
//!
//! \par USART configuration example.
//! In order to use this configuration class one must create configuration class
//! in the `ecl` namespace before any acccess to \ref usart_bus instance.
//! \code{.cpp}
//!    template<>
//!    struct usart_cfg< usart_device::dev3 >
//!    {
//!        static auto constexpr baudrate  = 115200;
//!        static auto constexpr word_len  = USART_WordLength_8b;
//!        static auto constexpr stop_bit  = USART_StopBits_1;
//!        static auto constexpr parity    = USART_Parity_No;
//!        static auto constexpr mode      = USART_Mode_Rx | USART_Mode_Tx;
//!        static auto constexpr hw_flow   = USART_HardwareFlowControl_None;
//!    };
//! \endcode
//! \warning To avoid potential problems with multiple configurations for single
//! USART bus, **make sure that full specialization is placed in the
//! header included (directly or indirectly) by all dependent modules.**.
//! Thus, redefinition of the config class for given USART will result in
//! compilation errors. *Good practice is to place all USART configuration
//! class in the single target-related header.*
template<usart_device dev>
struct usart_cfg
{
    // Always assert
    static_assert(std::is_integral<decltype(dev)>::value,
                  "The instance of this generic class should never be "
                  "instantiated. Please write your own template specialization "
                  "of this class. See documentation.");
};

struct bypass_console;

//! \brief STM32 USART bus
//!
template<usart_device dev>
class usart_bus
{
    // Reuses usart_bus in order to initialize bypass console driver
    friend struct bypass_console;
public:
    // Convenient type aliases.
    using channel       = ecl::bus_channel;
    using event         = ecl::bus_event;
    using handler_fn    = ecl::bus_handler;

    // Static interface only
    usart_bus() = delete;
    ~usart_bus() = delete;

    //! \brief Lazy initialization.
    //! \return Status of operation.
    static ecl::err init();

    //! \brief Sets rx buffer with given size.
    //! \param[in,out]  rx      Buffer to write data to. Optional.
    //! \param[in]      size    Size
    static void set_rx(uint8_t *rx, size_t size);

    //! \brief Sets tx buffer made-up from sequence of similar bytes.
    //! \param[in] size         Size of sequence
    //! \param[in] fill_byte    Byte to fill a sequence. Optional.
    static void set_tx(size_t size, uint8_t fill_byte = 0xff);

    //! \brief Sets tx buffer with given size.
    //! \param[in] tx   Buffer to transmit. Optional.
    //! \param[in] size Buffer size.
    static void set_tx(const uint8_t *tx, size_t size);

    //! \brief Sets event handler.
    //! Handler will be used by the bus, until reset_handler() will be called.
    //! \param[in] handler Handler itself.
    static void set_handler(const handler_fn &handler);

    //! \brief Reset xfer buffers.
    //! Buffers that were set by \sa set_tx() and \sa set_rx()
    //! will be no longer used after this call.
    static void reset_buffers();

    //! \brief Resets previously set handler.
    static void reset_handler();

    //! \brief Executes xfer, using buffers previously set.
    //! When it will be done, handler will be invoked.
    //! \return Status of operation.
    static ecl::err do_xfer();

    //! \brief Executes rx transfer, using rx buffer previously set.
    //! When it will be done, handler will be invoked.
    //! \return Status of operation.
    static ecl::err do_rx();

    //! \brief Executes tx transfer, using tx buffer previously set.
    //! When it will be done, handler will be invoked.
    //! \return Status of operation.
    static ecl::err do_tx();

    //! \brief Cancels xfer.
    //! After this call no xfer will occur.
    //! \return Status of operation.
    static ecl::err cancel_xfer();

    //! \brief Cancels xfer on rx.
    //! After this call no rx transfer will occur.
    //! \return Status of opetation.
    static ecl::err cancel_rx();

    //! \brief Cancels xfer on tx.
    //! After this call no tx transfer will occur.
    //! \return Status of opetation.
    static ecl::err cancel_tx();

    //! \brief Enables listen mode.
    //! In listen mode, UART bus uses different semantics of the `transfer complete`
    //! event. `transfer complete` will be generated with every byte received.
    //! bus_handler will be called with `total` argument that equal to current
    //! count of bytes written to the buffer.
    //! \note Can be called from ISR.
    static ecl::err enable_listen_mode();

    //! \brief Disables listen mode.
    //! \note Can be called from ISR.
    //! Changes buffer usage and `transfer compelte` event semantics back to normal.
    static ecl::err disable_listen_mode();

    // Should not be copied.
    usart_bus &operator=(usart_bus&) = delete;
    usart_bus(usart_bus&) = delete;

private:
    //! Picks proper RCC at compile time.
    static auto pick_rcc();
    //! Picks proper RCC operation function at compile time.
    static auto pick_rcc_fn();
    //! Picks IRQ number at compile time.
    static auto pick_irqn();
    //! Converts to proper USART type.
    static auto pick_usart();

    // Device status flags

    //! Bit set if device initialized.
    static constexpr uint8_t m_inited      = 0x1;
    //! Bit set if tx is done.
    static constexpr uint8_t m_tx_done     = 0x2;
    //! Bit set if rx is done.
    static constexpr uint8_t m_rx_done     = 0x4;
    //! Bit set if listen mode is enabled.
    static constexpr uint8_t m_listen_mode = 0x8;
    //! Bit set if rx transfer was canceled.
    static constexpr uint8_t m_rx_canceled = 0x10;
    //! Bit set if rx transfer was canceled.
    static constexpr uint8_t m_tx_canceled = 0x20;

    // Device status methods

    static inline bool inited()           { return m_status & m_inited; }
    static inline bool tx_done()          { return m_status & m_tx_done; }
    static inline bool rx_done()          { return m_status & m_rx_done; }
    static inline bool listen_mode()      { return m_status & m_listen_mode; }
    static inline bool rx_canceled()         { return m_status & m_rx_canceled; }
    static inline bool tx_canceled()         { return m_status & m_tx_canceled; }

    static inline void set_inited()       { m_status |= m_inited; }
    static inline void set_tx_done()      { m_status |= m_tx_done; }
    static inline void set_rx_done()      { m_status |= m_rx_done; }
    static inline void set_listen_mode()  { m_status |= m_listen_mode; }
    static inline void set_rx_canceled()     { m_status |= m_rx_canceled; }
    static inline void set_tx_canceled()     { m_status |= m_tx_canceled; }

    static inline void clear_inited()     { m_status &= ~(m_inited); }
    static inline void clear_tx_done()    { m_status &= ~(m_tx_done); }
    static inline void clear_rx_done()    { m_status &= ~(m_rx_done); }
    static inline void clear_listen_mode(){ m_status &= ~(m_listen_mode); }
    static inline void clear_rx_canceled()   { m_status &= ~(m_rx_canceled); }
    static inline void clear_tx_canceled()   { m_status &= ~(m_tx_canceled); }

    //! Handles IRQ events from a bus.
    static void irq_handler();

    //! Stores Handler passed via set_handler()
    static std::aligned_storage_t<sizeof(handler_fn), alignof(handler_fn)> m_handler_storage;

    //! Gets event handler
    static constexpr auto &event_handler();

    static const uint8_t   *m_tx;           //! Transmit buffer.
    static size_t          m_tx_size;       //! TX buffer size.
    static size_t          m_tx_left;       //! Left to send in TX buffer.
    static uint8_t         *m_rx;           //! Receive buffer.
    static size_t          m_rx_size;       //! RX buffer size.
    static size_t          m_rx_left;       //! Left to receive in RX buffer.
    static uint8_t         m_status;        //! Tracks device status.
};

// Static members declarations

template<usart_device dev>
std::aligned_storage_t<sizeof(typename usart_bus<dev>::handler_fn), alignof(typename usart_bus<dev>::handler_fn)>
usart_bus<dev>::m_handler_storage;

template<usart_device dev>
const uint8_t *usart_bus<dev>::m_tx;

template<usart_device dev>
uint8_t *usart_bus<dev>::m_rx;

template<usart_device dev>
size_t usart_bus<dev>::m_tx_size;

template<usart_device dev>
size_t usart_bus<dev>::m_tx_left;

template<usart_device dev>
size_t usart_bus<dev>::m_rx_size;

template<usart_device dev>
size_t usart_bus<dev>::m_rx_left;

template<usart_device dev>
uint8_t usart_bus<dev>::m_status;

template<usart_device dev>
ecl::err usart_bus<dev>::init()
{
    USART_InitTypeDef init_struct;

    // Already initialized.
    if (inited()) {
        return ecl::err::ok;
    }

    // Init static storage

    new (&m_handler_storage) handler_fn;

    // SPL-like checks, but in compile time.

    static_assert(IS_USART_WORD_LENGTH(usart_cfg<dev>::word_len),
                  "Word length configuration is invalid");
    static_assert(IS_USART_STOPBITS(usart_cfg<dev>::stop_bit),
                  "Stop bits configuration is invalid");
    static_assert(IS_USART_PARITY(usart_cfg<dev>::parity),
                  "Parity configuration is invalid");
    static_assert(IS_USART_MODE(usart_cfg<dev>::mode),
                  "USART mode is invalid");
    static_assert(IS_USART_HARDWARE_FLOW_CONTROL(usart_cfg<dev>::hw_flow),
                  "Flow control configuration is invalid");

    // Should be optimized at compile time
    auto rcc_periph = pick_rcc();
    auto rcc_fn     = pick_rcc_fn();
    auto usart      = pick_usart();
    auto irqn       = pick_irqn();

    // Enable peripheral clock
    rcc_fn(rcc_periph, ENABLE);

    // Configure UART
    init_struct.USART_BaudRate             = usart_cfg<dev>::baudrate;
    init_struct.USART_WordLength           = usart_cfg<dev>::word_len;
    init_struct.USART_StopBits             = usart_cfg<dev>::stop_bit;
    init_struct.USART_Parity               = usart_cfg<dev>::parity;
    init_struct.USART_Mode                 = usart_cfg<dev>::mode;
    init_struct.USART_HardwareFlowControl  = usart_cfg<dev>::hw_flow;

    // Init UART
    USART_Init(usart, &init_struct);

    // TODO: enable irq before each transaction and disable after
    // rather than keep it enabled all time
    auto lambda = []() {
        irq_handler();
    };

    irq::subscribe(irqn, lambda);
    irq::unmask(irqn);

    set_inited();

    // Enable UART
    USART_Cmd(usart, ENABLE);

    return ecl::err::ok;
}

template<usart_device dev>
void usart_bus<dev>::set_rx(uint8_t *rx, size_t size)
{
    ecl_assert(inited());

    m_rx = rx;
    m_rx_size = size;
}

template<usart_device dev>
void usart_bus<dev>::set_tx(size_t size, uint8_t fill_byte)
{
    // TODO: implement
    ecl_assert_msg(0, "Not implemented!");

    (void) size;
    (void) fill_byte;
}

template<usart_device dev>
void usart_bus<dev>::set_tx(const uint8_t *tx, size_t size)
{
    ecl_assert(inited());

    // TODO: assert if not initialized
    m_tx = tx;
    m_tx_size = size;
}


template<usart_device dev>
void usart_bus<dev>::set_handler(const handler_fn &handler)
{
    ecl_assert(inited());
    event_handler() = handler;
}

template<usart_device dev>
void usart_bus<dev>::reset_buffers()
{
    ecl_assert(inited());

    m_tx = nullptr;
    m_rx = nullptr;
    m_tx_left = m_tx_size = 0;
    m_rx_left = m_rx_size = 0;

    clear_rx_done();
    clear_tx_done();
}

template<usart_device dev>
void usart_bus<dev>::reset_handler()
{
    ecl_assert(inited());

    event_handler() = handler_fn{};
}

template<usart_device dev>
ecl::err usart_bus<dev>::do_xfer()
{
    // Order matters
    do_rx();
    do_tx();
    
    return ecl::err::ok;
}

template<usart_device dev>
ecl::err usart_bus<dev>::do_rx()
{
    ecl_assert(inited());

    auto usart = pick_usart();

    if (!m_rx) {
        set_rx_done();
        return ecl::err::ok;
    }

    m_rx_left = m_rx_size;

    clear_rx_done();

    // In case if previous xfer was canceled.
    clear_rx_canceled();

    // Bytes will be send in IRQ handler.
    USART_ITConfig(usart, USART_IT_RXNE, ENABLE);

    return ecl::err::ok;
}

template<usart_device dev>
ecl::err usart_bus<dev>::do_tx()
{
    ecl_assert(inited());

    auto usart = pick_usart();
    //auto irqn  = pick_irqn();

    if (!m_tx) {
        set_tx_done();
        return ecl::err::ok;
    }

    m_tx_left = m_tx_size;
    
    clear_tx_done();

    // In case if previous xfer was canceled.
    clear_tx_canceled();

    // Bytes will be send in IRQ handler.
    USART_ITConfig(usart, USART_IT_TXE, ENABLE);

    //irq::unmask(irqn);

    return ecl::err::ok;
}

template<usart_device dev>
ecl::err usart_bus<dev>::cancel_xfer()
{
    cancel_rx();
    cancel_tx();

    return ecl::err::ok;
}

template<usart_device dev>
ecl::err usart_bus<dev>::cancel_rx()
{
    ecl_assert(inited());

    auto usart = pick_usart();
    auto irqn  = pick_irqn();

    USART_ITConfig(usart, USART_IT_RXNE, DISABLE);

    //irq::mask(irqn);
    irq::clear(irqn);

    set_rx_done();
    set_rx_canceled();

    return ecl::err::ok;
}

template<usart_device dev>
ecl::err usart_bus<dev>::cancel_tx()
{
    ecl_assert(inited());

    auto usart = pick_usart();
    auto irqn  = pick_irqn();

    USART_ITConfig(usart, USART_IT_TXE, DISABLE);

    //irq::mask(irqn);
    irq::clear(irqn);

    set_tx_done();
    set_tx_canceled();

    return ecl::err::ok;
}

template<usart_device dev>
ecl::err usart_bus<dev>::enable_listen_mode()
{
    // Meaningfull if RX is disabled
    ecl_assert(m_rx && m_rx_size);

    set_listen_mode();
    return ecl::err::ok;
}

template<usart_device dev>
ecl::err usart_bus<dev>::disable_listen_mode()
{
    // Meaningfull if listen mode is already disabled
    ecl_assert(listen_mode());
    clear_listen_mode();

    return ecl::err::ok;
}

// -----------------------------------------------------------------------------
// Private members

template<usart_device dev>
constexpr auto &usart_bus<dev>::event_handler()
{
    return reinterpret_cast<handler_fn&>(m_handler_storage);
}

template<usart_device dev>
auto usart_bus<dev>::pick_rcc()
{
    // USART1 and USART6 are on APB2
    // USART2, USART3, UART4, UART5 are on APB1
    // See datasheet for detailed explanations
    // TODO: Switch instead of 'if's
    auto usart = pick_usart();
    if (usart == USART1)
        return RCC_APB2Periph_USART1;
    if (usart == USART2)
        return RCC_APB1Periph_USART2;
    if (usart == USART3)
        return RCC_APB1Periph_USART3;
    if (usart == UART4)
        return RCC_APB1Periph_UART4;
    if (usart == UART5)
        return RCC_APB1Periph_UART5;
#ifdef USART6
    if (usart == USART6)
        return RCC_APB2Periph_USART6;
#endif
    return static_cast< uint32_t >(-1);
}

template<usart_device dev>
auto usart_bus<dev>::pick_rcc_fn()
{
    // USART1 and USART6 are on APB2
    // USART2, USART3, UART4, UART5 are on APB1
    // See datasheet for detailed explanations
    auto usart = pick_usart();
    // TODO: switch instead of 'if's
    if (usart == USART1
#ifdef USART6
        || usart == USART6
#endif
        ) {
        return RCC_APB2PeriphClockCmd;
    } else if (usart == USART2 || usart == USART3 ||
               usart == UART4  || usart == UART5) {
        return RCC_APB1PeriphClockCmd;
    } else {
        // TODO: clarify
        return static_cast<decltype(&RCC_APB1PeriphClockCmd)>(nullptr);
    }
}

template<usart_device dev>
auto usart_bus<dev>::pick_irqn()
{
    auto usart = pick_usart();

    if (usart == USART1) {
        return USART1_IRQn;
    } else if (usart == USART2) {
        return USART2_IRQn;
    } else if (usart == USART3) {
        return USART3_IRQn;
    } else if (usart == UART4) {
        return UART4_IRQn;
    } else if (usart == UART5) {
        return UART5_IRQn;
#ifdef USART6
    } else if (usart == USART6) {
        return USART6_IRQn;
#endif
    } else {
        return static_cast<IRQn>(-1);
    }
}

template<usart_device dev>
auto usart_bus<dev>::pick_usart()
{
    switch (dev) {
    case usart_device::dev1:
        return USART1;
    case usart_device::dev2:
        return USART2;
    case usart_device::dev3:
        return USART3;
    case usart_device::dev4:
        return UART4;
    case usart_device::dev5:
        return UART5;
#ifdef USART6
    case usart_device::dev6:
        return USART6;
#endif
        // TODO: clarify
    default:
        return static_cast< decltype(USART1) >(nullptr);
    };
}

template<usart_device dev>
void usart_bus<dev>::irq_handler()
{
    auto usart = pick_usart();
    auto irqn  = pick_irqn();

    irq::clear(irqn);

    // TODO: comment about flags clear sequence

    if (!tx_done()) {
        if (USART_GetITStatus(usart, USART_IT_TXE) == SET && m_tx) {
            if (m_tx_left) {
                USART_SendData(usart, m_tx[m_tx_size - m_tx_left--]);
            } else {
                // Last interrupt occurred, need to notify.
                event_handler()(channel::tx, event::tc, m_tx_size);

                // Transaction complete.
                set_tx_done();
                USART_ITConfig(usart, USART_IT_TXE, DISABLE);
            }
        }
    }

    // No need to wait tx_done flag if in listen mode. It is async by nature.
    // Otherwise (regular mode) - wait till TX is done.
    if (listen_mode() || (tx_done() && !rx_done())) {
        if (USART_GetITStatus(usart, USART_IT_RXNE) == SET && m_rx) {
            auto data = USART_ReceiveData(usart);

            m_rx[m_rx_size - m_rx_left--] = static_cast<uint8_t>(data);

            if (!m_rx_left) { // RX is over.
                // Transaction complete.
                set_rx_done();
                USART_ITConfig(usart, USART_IT_RXNE, DISABLE);

                // Notify user
                event_handler()(channel::rx, event::tc, m_rx_size);
            } else if (listen_mode()) { // RX is not over, but listen mode
                                        // enabled. User must be notified
                                        // for every byte received..
                // Notify about state of the buffer.
                event_handler()(channel::rx, event::tc, m_rx_size - m_rx_left);
            }
        }
    }

    if (tx_done() && rx_done()) {
        // TODO: clear 'done' flags here too instead doing it in do_rx()/do_tx() ?
        if (!tx_canceled() && !rx_canceled()) {
            // Both TX and RX are finished. Notifying.
            event_handler()(channel::meta, event::tc, 0);
        }
    } 

    // Keep interrupts always on
    irq::unmask(irqn);
}

} // namespace ecl


#endif // PLATFORM_USART_BUS_HPP_
