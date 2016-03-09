//!
//! \file
//! \brief STM32F4xx USART driver
//!

#ifndef PLATFORM_USART_BUS_HPP_
#define PLATFORM_USART_BUS_HPP_


#include <common/usart.hpp>
#include <stm32f4xx_usart.h>
#include <stm32f4xx_rcc.h>

#include <cstdint>
#include <unistd.h>

#include <functional>
#include <platform/irq_manager.hpp>

#include <ecl/err.hpp>

//!
//! \brief STM32F4 USART bus
//!
template< usart_device dev >
class usart_bus
{
public:
    //!
    //! \brief Channles of a bus.
    //!
    enum class channel
    {
        rx,         //!< Receive channel.
        tx,         //!< Transmit channel.
        meta,       //!< Meta-channel.
    };

    //!
    //! \brief Various events.
    //!
    enum class event
    {
        ht,         //!< Half transfer event.
        tc,         //!< Transfer complete event.
        err,        //!< Error event.
    };


    //!
    //! \brief Externally supplied event handler.
    //!
    using handler_fn
    = std::function< void(channel ch, event type, size_t total) >;

    //!
    //! \brief Constructs a bus.
    //!
    usart_bus();

    //!
    //! \brief Destructs a bus.
    ~usart_bus();

    //!
    //! \brief Lazy initialization.
    //! \return Status of opeartion.
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
    //! \param[in] size Size of sequence
    //! \param fill_byte
    //!
    void set_tx(size_t size, uint8_t fill_byte);

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

    //! Handles IRQ events from a bus.
    void irq_handler();

    handler_fn      m_event_handler; //! Handler passed via set_handler()
    const uint8_t   *m_tx;           //! Transmit buffer
    uint8_t         *m_rx;           //! Recieve buffer
};

template< usart_device dev >
usart_bus< dev >::usart_bus()
    :m_event_handler{}
    ,m_tx{nullptr}
    ,m_rx{nullptr}
{

}

template< usart_device dev >
usart_bus< dev >::~usart_bus()
{

}

template< usart_device dev >
ecl::err usart_bus< dev >::init()
{
    USART_InitTypeDef initStruct;

    // TODO: implement
//    if (m_inited)
//        return -1;

    // Must be optimized at compile time
    constexpr auto RCC_Periph = pick_rcc();
    constexpr auto RCC_fn     = pick_rcc_fn();
    constexpr auto usart      = pick_usart();

    // Enable peripheral clock
    RCC_fn(RCC_Periph, ENABLE);

    // Configure UART
    // TODO: make configuration values be chosen at compile time
    initStruct.USART_BaudRate             = 115200;
    initStruct.USART_WordLength           = USART_WordLength_8b;
    initStruct.USART_StopBits             = USART_StopBits_1;
    initStruct.USART_Parity               = USART_Parity_No;
    initStruct.USART_Mode                 = USART_Mode_Rx | USART_Mode_Tx;
    initStruct.USART_HardwareFlowControl  = USART_HardwareFlowControl_None;

    // Init UART
    USART_Init(usart, &initStruct);
//    m_inited = 1;

    // TODO: disable UART interrupts in NVIC

    // TODO: add static assert for supported modes and flags
    // Intended to be optimized at compile time

    USART_ITConfig(usart, USART_IT_ERR, ENABLE);
    USART_ITConfig(usart, USART_IT_TXE, ENABLE);
    USART_ITConfig(usart, USART_IT_RXNE, ENABLE);

    // Enable UART
    USART_Cmd(usart, ENABLE);

//    return 0;
    return ecl::err::ok;
}

template< usart_device dev >
void usart_bus< dev >::set_rx(uint8_t *rx, size_t size)
{
    (void) rx;
    (void) size;
}

template< usart_device dev >
void usart_bus< dev >::set_tx(const uint8_t *tx, size_t size)
{
    (void) tx;
    (void) size;
}

template< usart_device dev >
void usart_bus< dev >::set_handler(const handler_fn &handler)
{
    (void) handler;
}

template< usart_device dev >
void usart_bus< dev >::reset_buffers()
{

}

template< usart_device dev >
void usart_bus< dev >::reset_handler()
{

}

template< usart_device dev >
ecl::err usart_bus< dev >::do_xfer()
{
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

}


#endif // PLATFORM_USART_BUS_HPP_
