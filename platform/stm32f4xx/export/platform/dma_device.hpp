#ifndef PLATFORM_DMA_DEVICE
#define PLATFORM_DMA_DEVICE

#include <stm32f4xx_dma.h>
#include <stm32f4xx_rcc.h>
#include <functional>

//------------------------------------------------------------------------------

namespace ecl
{

namespace dma
{

//!
//! \brief Converts DMA stream from integral to pointer form.
//!
template< std::uintptr_t dma_stream >
constexpr auto get_stream()
{
    return reinterpret_cast< DMA_Stream_TypeDef* >(dma_stream);
}

//!
//! \brief Gets stream number of given DMA stream.
//!
template< std::uintptr_t dma_stream >
constexpr auto get_stream_number()
{
    constexpr auto stream = get_stream< dma_stream >();

    if (stream == DMA1_Stream0 || stream == DMA2_Stream0) {
        return 0;
    } else if (stream == DMA1_Stream1 || stream == DMA2_Stream1) {
        return 1;
    } else if (stream == DMA1_Stream2 || stream == DMA2_Stream2) {
        return 2;
    } else if (stream == DMA1_Stream3 || stream == DMA2_Stream3) {
        return 3;
    } else if (stream == DMA1_Stream4 || stream == DMA2_Stream4) {
        return 4;
    } else if (stream == DMA1_Stream5 || stream == DMA2_Stream5) {
        return 5;
    } else if (stream == DMA1_Stream6 || stream == DMA2_Stream6) {
        return 6;
    } else if (stream == DMA1_Stream7 || stream == DMA2_Stream7) {
        return 7;
    }
}

//!
//! \brief Gets error flag descriptor of DMA stream.
//!
template< std::uintptr_t dma_stream >
constexpr auto get_err_flag()
{
    constexpr auto stream_no = get_stream< dma_stream >();

    switch (stream_no) {
    case 0:
        return DMA_FLAG_TEIF0;
    case 1:
        return DMA_FLAG_TEIF1;
    case 2:
        return DMA_FLAG_TEIF2;
    case 3:
        return DMA_FLAG_TEIF3;
    case 4:
        return DMA_FLAG_TEIF4;
    case 5:
        return DMA_FLAG_TEIF5;
    case 6:
        return DMA_FLAG_TEIF6;
    case 7:
        return DMA_FLAG_TEIF7;
    default:
        return static_cast< decltype (DMA_FLAG_TEIF7) >(-1);
    }
}

//!
//! \brief Gets half-transfer flag descriptor of DMA stream.
//!
template< std::uintptr_t dma_stream >
constexpr auto get_ht_flag()
{
    constexpr auto stream_no = get_stream< dma_stream >();

    switch (stream_no) {
    case 0:
        return DMA_FLAG_HTIF0;
    case 1:
        return DMA_FLAG_HTIF1;
    case 2:
        return DMA_FLAG_HTIF2;
    case 3:
        return DMA_FLAG_HTIF3;
    case 4:
        return DMA_FLAG_HTIF4;
    case 5:
        return DMA_FLAG_HTIF5;
    case 6:
        return DMA_FLAG_HTIF6;
    case 7:
        return DMA_FLAG_HTIF7;
    default:
        return 0xfful; // TODO
    }

}

//!
//! \brief Gets transfer complete flag descriptor using of DMA stream.
//!
template< std::uintptr_t dma_stream >
constexpr auto get_tc_flag()
{
    constexpr auto stream_no = get_stream_number< dma_stream >();

    switch (stream_no) {
    case 0:
        return DMA_FLAG_TCIF0;
    case 1:
        return DMA_FLAG_TCIF1;
    case 2:
        return DMA_FLAG_TCIF2;
    case 3:
        return DMA_FLAG_TCIF3;
    case 4:
        return DMA_FLAG_TCIF4;
    case 5:
        return DMA_FLAG_TCIF5;
    case 6:
        return DMA_FLAG_TCIF6;
    case 7:
        return DMA_FLAG_TCIF7;
    default:
        return 0xfful; // TODO
    }
}

//!
//! \brief Gets IRQ number of given DMA stream.
//!
template< std::uintptr_t dma_stream >
constexpr auto get_irqn()
{
    constexpr auto stream = get_stream< dma_stream >();

    if (stream == DMA1_Stream0)      // DMA1
        return DMA1_Stream0_IRQn;
    else if (stream == DMA1_Stream1)
        return DMA1_Stream1_IRQn;
    else if (stream == DMA1_Stream2)
        return DMA1_Stream2_IRQn;
    else if (stream == DMA1_Stream3)
        return DMA1_Stream3_IRQn;
    else if (stream == DMA1_Stream4)
        return DMA1_Stream4_IRQn;
    else if (stream == DMA1_Stream5)
        return DMA1_Stream5_IRQn;
    else if (stream == DMA1_Stream6)
        return DMA1_Stream6_IRQn;
    else if (stream == DMA1_Stream7)
        return DMA1_Stream7_IRQn;
    else if (stream == DMA2_Stream0)  // DMA2
        return DMA2_Stream0_IRQn;
    else if (stream == DMA2_Stream1)
        return DMA2_Stream1_IRQn;
    else if (stream == DMA2_Stream2)
        return DMA2_Stream2_IRQn;
    else if (stream == DMA2_Stream3)
        return DMA2_Stream3_IRQn;
    else if (stream == DMA2_Stream4)
        return DMA2_Stream4_IRQn;
    else if (stream == DMA2_Stream5)
        return DMA2_Stream5_IRQn;
    else if (stream == DMA2_Stream6)
        return DMA2_Stream6_IRQn;
    else if (stream == DMA2_Stream7)
        return DMA2_Stream7_IRQn;
    else
        return static_cast< decltype (DMA1_Stream0_IRQn) >(-1);
}

//!
//! \brief Gets reset and clock control descriptor associated with a given stream.
//!
template< std::uintptr_t dma_stream >
constexpr auto get_rcc()
{
    constexpr auto stream = get_stream< dma_stream >();

    if (stream == DMA1_Stream0
            || stream == DMA1_Stream1
            || stream == DMA1_Stream2
            || stream == DMA1_Stream3
            || stream == DMA1_Stream4
            || stream == DMA1_Stream5
            || stream == DMA1_Stream6
            || stream == DMA1_Stream7) {
        return RCC_AHB1Periph_DMA1;
    } else {
        return RCC_AHB1Periph_DMA2;
    }

}

//!
//! \brief Gets half-transfer interrupt flag descriptor of given dma stream.
//!
template< std::uintptr_t dma_stream >
constexpr auto get_ht_if()
{
    constexpr auto stream_no = get_stream_number< dma_stream >();

    switch (stream_no) {
    case 0:
        return DMA_IT_HTIF0;
    case 1:
        return DMA_IT_HTIF1;
    case 2:
        return DMA_IT_HTIF2;
    case 3:
        return DMA_IT_HTIF3;
    case 4:
        return DMA_IT_HTIF4;
    case 5:
        return DMA_IT_HTIF5;
    case 6:
        return DMA_IT_HTIF6;
    case 7:
        return DMA_IT_HTIF7;
    default:
        return 0xfful; // TODO: ugly
    }

}

//!
//! \brief Gets transfer complete interrupt flag descriptor of given dma stream.
//!
template< std::uintptr_t dma_stream >
constexpr auto get_tc_if()
{
    constexpr auto stream_no = get_stream_number< dma_stream >();

    switch (stream_no) {
    case 0:
        return DMA_IT_TCIF0;
    case 1:
        return DMA_IT_TCIF1;
    case 2:
        return DMA_IT_TCIF2;
    case 3:
        return DMA_IT_TCIF3;
    case 4:
        return DMA_IT_TCIF4;
    case 5:
        return DMA_IT_TCIF5;
    case 6:
        return DMA_IT_TCIF6;
    case 7:
        return DMA_IT_TCIF7;
    default:
        return 0xfful; // TODO
    }
}

//!
//! \brief Gets transfer error interrupt flag descriptor of given dma stream.
//!
template< std::uintptr_t dma_stream >
constexpr auto get_err_if()
{
    constexpr auto stream_no = get_stream_number< dma_stream >();

    switch (stream_no) {
    case 0:
        return DMA_IT_TEIF0 | DMA_IT_DMEIF0 | DMA_IT_FEIF0;
    case 1:
        return DMA_IT_TEIF1 | DMA_IT_DMEIF1 | DMA_IT_FEIF1;
    case 2:
        return DMA_IT_TEIF2 | DMA_IT_DMEIF2 | DMA_IT_FEIF2;
    case 3:
        return DMA_IT_TEIF3 | DMA_IT_DMEIF3 | DMA_IT_FEIF3;
    case 4:
        return DMA_IT_TEIF4 | DMA_IT_DMEIF4 | DMA_IT_FEIF4;
    case 5:
        return DMA_IT_TEIF5 | DMA_IT_DMEIF5 | DMA_IT_FEIF5;
    case 6:
        return DMA_IT_TEIF6 | DMA_IT_DMEIF6 | DMA_IT_FEIF6;
    case 7:
        return DMA_IT_TEIF7 | DMA_IT_DMEIF7 | DMA_IT_FEIF7;
    default:
        return static_cast< decltype (DMA_IT_TEIF7) >(-1);
    }
}

//!
//! \brief Initializes DMA peripherial.
//!
template< std::uintptr_t dma_stream >
void init_rcc()
{
    // stm32f4 DMA is placed on AHB1 bus only
    constexpr auto dma_rcc = get_rcc< dma_stream >();
    RCC_AHB1PeriphClockCmd(dma_rcc, ENABLE);
}

//!
//! \brief Enables IRQ for given DMA stream and interrupt sources.
//!
template< std::uintptr_t dma_stream, uint32_t flags >
void enable_irq()
{
    constexpr auto stream   = get_stream< dma_stream >();

    // Enable interrupt sources
    DMA_ITConfig(stream, flags, ENABLE);
}

//!
//! \brief Disables IRQ for given DMA stream and interrupt sources.
//!
template< std::uintptr_t dma_stream, uint32_t flags >
void disable_irq()
{
    constexpr auto stream   = get_stream< dma_stream >();

    // Enable interrupt sources
    DMA_ITConfig(stream, flags, DISABLE);
}

//!
//! \brief Subscribes to DMA IRQ associated with given DMA stream.
//!
template< std::uintptr_t dma_stream >
void subscribe_irq(const std::function< void () > &handler)
{
    constexpr auto dma_irqn = get_irqn< dma_stream >();

    // Disable interrupts when configuring interrupts
    IRQ_manager::mask(dma_irqn);

    // Do not expose old, not handled interrupts
    IRQ_manager::clear(dma_irqn);

    // Subscribe, actually
    IRQ_manager::subscribe(dma_irqn, handler);

    // Go on
    // TODO: think about letting user to decide when to unmask interrupts
    IRQ_manager::unmask(dma_irqn);
}

} // namespace dma

} // namespace ecl

#endif
