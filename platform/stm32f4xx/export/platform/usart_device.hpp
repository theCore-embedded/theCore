// TODO: use better name for this module
#ifndef USART_HANDLER_HPP
#define USART_HANDLER_HPP

#include <common/usart.hpp>

#include <stm32f4xx_usart.h>
#include <stm32f4xx_rcc.h>

#include <cstdint>
#include <unistd.h>

#include <functional>
#include <platform/irq_manager.hpp>

// TODO: make it singleton since each HW device must have dedicated driver
// instance.
// TODO: think about making driver config (USART_TypeDef&)
// reusable in runtime, not compile time
template< usart_device	USARTx,
          USART_mode    mode        = USART_mode::poll,
          int32_t       flags       = USART_state::EMPTY >
class usart
{
    // Supported only IRQ and POLL modes.
    static_assert((mode == USART_mode::poll) ||
                  (mode == USART_mode::IRQ), "Invalid USART mode used!");
public:
    usart();
    ~usart();

    // Lazy initialization, -1 if error. 0 otherwise
    static int init();

    // -1 if error, 0 otherwise
    static int open();
    // -1 if error, 0 otherwise
    static int close();

    // -1 if error, [0, count] otherwise
    static ssize_t write(const uint8_t *data, size_t count);

    // -1 if error, [0, count] otherwise
    static ssize_t read(uint8_t *data, size_t count);

    // Queries peripheral status
    // INVALID if method failed, anything else from USART_state otherwise
    static int32_t get_state();

    // Clears state
    // If nothing is given then whole state is cleared
    static int clear_state(int32_t state);
    static int clear_state();

    // Set of routines to work with IRQs
    // -1 if error, 0 otherwise
    static int register_IRQ(const std::function< void() > &handler);

    // Completes IRQ for given status bits
    // If nothing is given then all pending interrupts are dropped
    // -1 if error, 0 otherwise
    static int complete_IRQ(int32_t state);
    static int complete_IRQ();

    // -1 if error, 0 otherwise
    static int deregisterIRQ();

private:
    // Picks proper RCC at compile time
    static constexpr auto pick_RCC();
    // Picks proper RCC operation function at compile time
    static constexpr auto pick_RCCfn();
    // Picks IRQ number at compile time
    static constexpr auto pick_IRQn();
    // Converts to proper USART type
    static constexpr auto pick_USART();

    // Convinient method
    static void init_IRQ();

    // Indicates that USART is ready for use
    static int m_inited;
    // Flag to prevent multiple opening
    static int m_opened;
};

// Interface
// -----------------------------------------------------------------------------

template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
usart< USARTx, mode, flags >::usart()
{
}

template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
usart< USARTx, mode, flags >::~usart()
{
    // TODO: implement
    if (m_inited) {
        // TODO
    }
}

template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
int usart< USARTx, mode, flags >::init()
{
    USART_InitTypeDef initStruct;

    // TODO: atomic cas?
    if (m_inited)
        return -1;

    // Must be optimized at compile time
    constexpr auto RCC_Periph = pick_RCC();
    constexpr auto RCC_fn     = pick_RCCfn();
    constexpr auto usart      = pick_USART();

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
    m_inited = 1;

    // TODO: disable UART interrupts in NVIC

    // TODO: add static assert for supported modes and flags
    // Intended to be optimized at compile time

    if (mode == USART_mode::IRQ) {
        init_IRQ();
    }

    return 0;
}

template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
int usart< USARTx, mode, flags >::m_inited = 0;

template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
int usart< USARTx, mode, flags >::m_opened = 0;


template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
int usart< USARTx, mode, flags >::open()
{
    if (!m_inited || m_opened)
        return -1;

    constexpr auto usart = pick_USART();

    // Enable UART
    USART_Cmd(usart, ENABLE);
    m_opened = 1;
    return 0;
}

template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
int usart< USARTx, mode, flags >::close()
{
    if (!m_opened)
        return -1;

    constexpr auto usart = pick_USART();

    // Disable UART
    USART_Cmd(usart, DISABLE);
    m_opened = 0;
    return 0;
}


template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
ssize_t usart< USARTx, mode, flags >::read(uint8_t *data, size_t count)
{
    if (!count)
        return 0;

    if (!m_opened)
        return -1;

    // TODO: implement support for IRQ mode

    constexpr auto usart = pick_USART();

    while (USART_GetFlagStatus(usart, USART_FLAG_RXNE) == RESET) {
        if (mode == USART_mode::IRQ) {
            // Do not wait anything in async mode.
            return -2;
        }
    }

    // USART HW buffer is only one byte long
    *data = USART_ReceiveData(usart);

    return 1;
}

template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
ssize_t usart< USARTx, mode, flags >::write(const uint8_t *data, size_t count)
{
    if (!count)
        return 0;

    if (!m_opened)
        return -1;

    // TODO: implement support for IRQ mode

    constexpr auto usart = pick_USART();

    if (mode == USART_mode::IRQ && USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET) {
        // Do not wait anything in async mode.
        return -2;
    }

    size_t counter = count;


    while (counter--) {
        while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET) { }
        // USART HW buffer is only one byte long
        USART_SendData(usart, *data++);
    }

    return count;
}

template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
int32_t usart< USARTx, mode, flags >::get_state()
{
    constexpr auto usart = pick_USART();

    int32_t status = USART_state::EMPTY;

    if (USART_GetFlagStatus(usart, USART_FLAG_TXE) == SET) {
        status |= USART_state::TXC;
    }

    if (USART_GetFlagStatus(usart, USART_FLAG_RXNE) == SET) {
        status |= USART_state::RXP;
    }

    if (USART_GetFlagStatus(usart, USART_FLAG_ORE) == SET) {
        status |= USART_state::ORE;
    }

    if (USART_GetFlagStatus(usart, USART_FLAG_NE) == SET) {
        status |= USART_state::NE;
    }

    if (USART_GetFlagStatus(usart, USART_FLAG_FE) == SET) {
        status |= USART_state::FE;
    }

    if (USART_GetFlagStatus(usart, USART_FLAG_PE) == SET) {
        status |= USART_state::PE;
    }

    return status;
}

template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
int usart< USARTx, mode, flags >::clear_state(int32_t state)
{
    constexpr auto usart = pick_USART();

    // TODO: make sure that state is cleared even if
    // 'special' software sequence is required.
    // See 'USART_ClearFlag()'
    if (state & USART_state::ERROR) {
        // Clear all errors
    }

    if (state & USART_state::RXP) {
        USART_ClearFlag(usart, USART_FLAG_RXNE);
    }

    if (state & USART_state::TXC) {
        USART_ClearFlag(usart, USART_FLAG_TC);
    }

    return 0;
}


template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
int usart< USARTx, mode, flags >::register_IRQ(const std::function< void() > &handler)
{
    // TODO
    constexpr auto IRQn = pick_IRQn();
    IRQ_manager::subscribe(IRQn, handler);
    IRQ_manager::unmask(IRQn);
    return 0;
}


template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
int usart< USARTx, mode, flags >::complete_IRQ(int32_t state)
{
    constexpr auto usart = pick_USART();
    constexpr auto IRQn = pick_IRQn();

    // TODO: make sure that state is cleared even if
    // 'special' software sequence is required.
    // See 'USART_ClearITPendingBit()'

    if (state & USART_state::ERROR) {
        // Clear all errors
    }

    if (state & USART_state::RXP) {
        USART_ClearITPendingBit(usart, USART_IT_RXNE);
    }

    if (state & USART_state::TXC) {
        USART_ClearITPendingBit(usart, USART_IT_TC);
    }

    IRQ_manager::clear(IRQn);
    IRQ_manager::unmask(IRQn);

    return 0;
}

template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
int usart< USARTx, mode, flags >::complete_IRQ()
{
    return complete_IRQ(USART_state::INVALID);
}


template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
int usart< USARTx, mode, flags >::deregisterIRQ()
{
    constexpr auto IRQn = pick_IRQn();
    IRQ_manager::mask(IRQn);
    IRQ_manager::unsubscribe(IRQn);

    return -1;
}

// Private members
// -----------------------------------------------------------------------------
template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
constexpr auto usart< USARTx, mode, flags >::pick_RCC()
{
    // USART1 and USART6 are on APB2
    // USART2, USART3, UART4, UART5 are on APB1
    // See datasheet for detailed explanations
    // TODO: Switch instead of 'if's
    constexpr auto usart = pick_USART();
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

template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
constexpr auto usart< USARTx, mode, flags >::pick_RCCfn()
{
    // USART1 and USART6 are on APB2
    // USART2, USART3, UART4, UART5 are on APB1
    // See datasheet for detailed explanations
    constexpr USART_TypeDef *usart = pick_USART();
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

template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
constexpr auto usart< USARTx, mode, flags >::pick_IRQn()
{
    constexpr auto usart = pick_USART();

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

template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
constexpr auto usart< USARTx, mode, flags >::pick_USART()
{
    switch (USARTx) {
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

template< usart_device USARTx,
          USART_mode mode,
          int32_t flags >
void usart< USARTx, mode, flags >::init_IRQ()
{
    constexpr auto usart = pick_USART();

    if ((flags & USART_state::ERROR)
            || (flags & USART_state::FE)
            || (flags & USART_state::NE)
            || (flags & USART_state::ORE)) {

        USART_ITConfig(usart, USART_IT_ERR, ENABLE);
    }

    if (flags & USART_state::TXC) {
        USART_ITConfig(usart, USART_IT_TXE, ENABLE);
    }

    if (flags & USART_state::RXP) {
        USART_ITConfig(usart, USART_IT_RXNE, ENABLE);
    }
}

#endif // USART_HANDLER_HPP
