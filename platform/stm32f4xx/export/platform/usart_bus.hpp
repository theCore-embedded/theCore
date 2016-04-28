//!
//! \file
//! \brief STM32F4xx USART driver
//!

#ifndef PLATFORM_USART_BUS_HPP_
#define PLATFORM_USART_BUS_HPP_

#include <platform/common/bus.hpp>
#include <common/usart.hpp>
#include <ecl/err.hpp>

#include <stm32f4xx_usart.h>
#include <stm32f4xx_rcc.h>

#include <platform/irq_manager.hpp>

#include <cstdint>
#include <unistd.h>

#include <functional>

namespace ecl
{

//!
//! \brief STM32F4 USART bus
//!
template< usart_device dev >
class usart_bus
{
public:
    // Convenient type aliases.
    using channel       = ecl::bus_channel;
    using event         = ecl::bus_event;
    using handler_fn    = ecl::bus_handler;

    //!
    //! \brief Constructs a bus.
    //!
    usart_bus();

    //!
    //! \brief Destructs a bus.
    ~usart_bus();

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

private:
    //! Picks proper RCC at compile time.
    static constexpr auto pick_rcc();
    //! Picks proper RCC operation function at compile time.
    static constexpr auto pick_rcc_fn();
    //! Picks IRQ number at compile time.
    static constexpr auto pick_irqn();
    //! Converts to proper USART type.
    static constexpr auto pick_usart();

    // Device status flags

    //! Bit set if device initialized.
    static constexpr uint8_t m_inited     = 0x1;
    //! Bit set if tx is done.
    static constexpr uint8_t m_tx_done    = 0x2;
    //! Bit set if rx is done.
    static constexpr uint8_t m_rx_done    = 0x4;

    // Device status methods

    inline bool inited() const     { return (m_status & m_inited)  != 0; }
    inline bool tx_done() const    { return (m_status & m_tx_done) != 0; }
    inline bool rx_done() const    { return (m_status & m_rx_done) != 0; }

    inline void set_inited()       { m_status |= m_inited; }
    inline void set_tx_done()      { m_status |= m_tx_done; }
    inline void set_rx_done()      { m_status |= m_rx_done; }

    inline void clear_inited()     { m_status &= ~(m_inited); }
    inline void clear_tx_done()    { m_status &= ~(m_tx_done); }
    inline void clear_rx_done()    { m_status &= ~(m_rx_done); }


    //! Handles IRQ events from a bus.
    void irq_handler();

    handler_fn      m_event_handler; //! Handler passed via set_handler().
    const uint8_t   *m_tx;           //! Transmit buffer.
    size_t          m_tx_size;       //! TX buffer size.
    size_t          m_tx_left;       //! Left to send in TX buffer.
    uint8_t         *m_rx;           //! Receive buffer.
    size_t          m_rx_size;       //! RX buffer size.
    size_t          m_rx_left;       //! Left to receive in RX buffer.
    uint8_t         m_status;        //! Tracks device status.
};

template< usart_device dev >
usart_bus< dev >::usart_bus()
    :m_event_handler{}
    ,m_tx{nullptr}
    ,m_tx_size{0}
    ,m_tx_left{0}
    ,m_rx{nullptr}
    ,m_rx_size{0}
    ,m_rx_left{0}
    ,m_status{0}
{

}

template< usart_device dev >
usart_bus< dev >::~usart_bus()
{

}

template< usart_device dev >
ecl::err usart_bus< dev >::init()
{
    USART_InitTypeDef init_struct;

    // Already initialized.
    if (inited()) {
        return ecl::err::ok;
    }

    // Must be optimized at compile time
    constexpr auto rcc_periph = pick_rcc();
    constexpr auto rcc_fn     = pick_rcc_fn();
    constexpr auto usart      = pick_usart();
    constexpr auto irqn       = pick_irqn();

    // Enable peripheral clock
    rcc_fn(rcc_periph, ENABLE);

    // Configure UART
    // TODO: make configuration values be chosen at compile time
    init_struct.USART_BaudRate             = 115200;
    init_struct.USART_WordLength           = USART_WordLength_8b;
    init_struct.USART_StopBits             = USART_StopBits_1;
    init_struct.USART_Parity               = USART_Parity_No;
    init_struct.USART_Mode                 = USART_Mode_Rx | USART_Mode_Tx;
    init_struct.USART_HardwareFlowControl  = USART_HardwareFlowControl_None;

    // Init UART
    USART_Init(usart, &init_struct);

    // TODO: enable irq before each transaction and disable after
    // rather than keep it enabled all time
    auto lambda = [this]() {
        this->irq_handler();
    };

    IRQ_manager::subscribe(irqn, lambda);

    // Enable UART
    USART_Cmd(usart, ENABLE);

    set_inited();
    return ecl::err::ok;
}

template< usart_device dev >
void usart_bus< dev >::set_rx(uint8_t *rx, size_t size)
{
    // TODO: assert if not initialized
    if (!inited()) {
        return;
    }

    m_rx = rx;
    m_rx_size = size;
}

template< usart_device dev >
void usart_bus< dev >::set_tx(size_t size, uint8_t fill_byte)
{
    if (!inited()) {
        return;
    }

    // TODO: assert if not initialized
    (void) size;
    (void) fill_byte;
}

template< usart_device dev >
void usart_bus< dev >::set_tx(const uint8_t *tx, size_t size)
{
    if (!inited()) {
        return;
    }

    // TODO: assert if not initialized
    m_tx = tx;
    m_tx_size = size;
}


template< usart_device dev >
void usart_bus< dev >::set_handler(const handler_fn &handler)
{
    // It is possible (and recommended) to set handler before bus init.
    m_event_handler = handler;
}

template< usart_device dev >
void usart_bus< dev >::reset_buffers()
{
    // TODO: assert if not initialized
    if (!inited()) {
        return;
    }

    m_tx = nullptr;
    m_rx = nullptr;
    m_tx_left = m_tx_size = 0;
    m_rx_left = m_rx_size = 0;

    clear_rx_done();
    clear_tx_done();
}

template< usart_device dev >
void usart_bus< dev >::reset_handler()
{
    m_event_handler = handler_fn{};
}

template< usart_device dev >
ecl::err usart_bus< dev >::do_xfer()
{
    // TODO: assert if not initialized
    if (!inited()) {
        return ecl::err::generic;
    }

    constexpr auto irqn = pick_irqn();
    constexpr auto usart = pick_usart();

    if (m_tx) {
        m_tx_left = m_tx_size;
        clear_tx_done();

        // Bytes will be send in IRQ handler.
        USART_ITConfig(usart, USART_IT_TXE, ENABLE);
    } else {
        // TX is not requested. Assuming that it is has been done some
        // time ago.
        set_tx_done();
    }

    if (m_rx) {
        clear_rx_done();
        USART_ITConfig(usart, USART_IT_RXNE, ENABLE);
    } else {
        // TX is not requested. Assuming that it is has been done some
        // time ago.
        set_rx_done();
    }

    IRQ_manager::unmask(irqn);

    return ecl::err::ok;
}

// -----------------------------------------------------------------------------
// Private members

template< usart_device dev >
constexpr auto usart_bus< dev >::pick_rcc()
{
    // USART1 and USART6 are on APB2
    // USART2, USART3, UART4, UART5 are on APB1
    // See datasheet for detailed explanations
    // TODO: Switch instead of 'if's
    constexpr auto usart = pick_usart();
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
    if (usart == USART6)
        return RCC_APB2Periph_USART6;

    return static_cast< uint32_t >(-1);
}

template< usart_device dev >
constexpr auto usart_bus< dev >::pick_rcc_fn()
{
    // USART1 and USART6 are on APB2
    // USART2, USART3, UART4, UART5 are on APB1
    // See datasheet for detailed explanations
    constexpr auto usart = pick_usart();
    // TODO: switch instead of 'if's
    if (usart == USART1 || usart == USART6) {
        return RCC_APB2PeriphClockCmd;
    } else if (usart == USART2 || usart == USART3 ||
               usart == UART4  || usart == UART5) {
        return RCC_APB1PeriphClockCmd;
    } else {
        // TODO: clarify
        return static_cast< decltype(&RCC_APB1PeriphClockCmd) >(nullptr);
    }
}

template< usart_device dev >
constexpr auto usart_bus< dev >::pick_irqn()
{
    constexpr auto usart = pick_usart();

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
    } else if (usart == USART6) {
        return USART6_IRQn;
    }
}

template< usart_device dev >
constexpr auto usart_bus< dev >::pick_usart()
{
    switch (dev) {
    case usart_device::dev_1:
        return USART1;
    case usart_device::dev_2:
        return USART2;
    case usart_device::dev_3:
        return USART3;
    case usart_device::dev_4:
        return UART4;
    case usart_device::dev_5:
        return UART5;
    case usart_device::dev_6:
        return USART6;
        // TODO: clarify
    default:
        return static_cast< decltype(USART1) >(nullptr);
    };
}

template< usart_device dev >
void usart_bus< dev >::irq_handler()
{
    constexpr auto usart = pick_usart();
    constexpr auto irqn  = pick_irqn();
    ITStatus status;

    IRQ_manager::clear(irqn);

    // TODO: comment about flags clear sequence

    if (!tx_done()) {
        status = USART_GetITStatus(usart, USART_IT_TXE);
        if (status == SET && m_tx) {
            if (m_tx_left) {
                USART_SendData(usart, *(m_tx + (m_tx_size - m_tx_left)));
                m_tx_left--;
                IRQ_manager::unmask(irqn);
            } else {
                // Last interrupt occurred, need to notify.
                m_event_handler(channel::tx, event::tc, m_tx_size);

                // Transaction complete.
                set_tx_done();
                USART_ITConfig(usart, USART_IT_TXE, DISABLE);
            }
        }
    } else if (!rx_done()) {  // Perform RX only after TX is finished.
        status = USART_GetITStatus(usart, USART_IT_RXNE);

        if (status == SET) {
            // Do not receive more than one byte. This is actually a small
            // adaptation to console purposes. Every symbol must be immediately
            // transfered to the client of the console driver (code that owns
            // this bus).
            // If throughput is the case, additional buffering may be applied,
            // so this usart bus will accumulate some data (in other words,
            // will buffer RX stream) even if the client not requesting anything.
            auto data = USART_ReceiveData(usart);
            *m_rx = static_cast< uint8_t >(data);

            // Notify about that 1 byte is received.
            m_event_handler(channel::rx, event::tc, 1);

            // Transaction complete.
            set_rx_done();
            USART_ITConfig(usart, USART_IT_RXNE, DISABLE);

            // 1 byte is received. No need to unmask interrupts.
        }
    }

    if (tx_done() && rx_done()) {
        // Both TX and RX are finished. Notifying.
        m_event_handler(channel::meta, event::tc, 0);
    }
}

}


#endif // PLATFORM_USART_BUS_HPP_
