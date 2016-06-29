//!
//! \file
//! \brief DMA STM32F4XX helpers
#ifndef PLATFORM_DMA_DEVICE
#define PLATFORM_DMA_DEVICE

#include <stm32f4xx_dma.h>
#include <stm32f4xx_rcc.h>
#include <functional>
#include <type_traits>

//------------------------------------------------------------------------------

namespace ecl
{

namespace dma
{

//! DMA streams present, according to the RM.
//! \details High-order byte represents DMA number, low order - stream number
//!
enum class streams
{
    dma1_0 = 0x100,
    dma1_1 = 0x101,
    dma1_2 = 0x102,
    dma1_3 = 0x103,
    dma1_4 = 0x104,
    dma1_5 = 0x105,
    dma1_6 = 0x106,
    dma1_7 = 0x107,
    dma2_0 = 0x200,
    dma2_1 = 0x201,
    dma2_2 = 0x202,
    dma2_3 = 0x203,
    dma2_4 = 0x204,
    dma2_5 = 0x205,
    dma2_6 = 0x206,
    dma2_7 = 0x207,
};

//!
//! \brief Maps stream numbers to the stream addresses.
//!
template< streams dma_stream >
static inline auto get_stream()
{
    switch (dma_stream) {
        case streams::dma1_0:
            return DMA1_Stream0;
        case streams::dma1_1:
            return DMA1_Stream1;
        case streams::dma1_2:
            return DMA1_Stream2;
        case streams::dma1_3:
            return DMA1_Stream3;
        case streams::dma1_4:
            return DMA1_Stream4;
        case streams::dma1_5:
            return DMA1_Stream5;
        case streams::dma1_6:
            return DMA1_Stream6;
        case streams::dma1_7:
            return DMA1_Stream7;
        case streams::dma2_0:
            return DMA2_Stream0;
        case streams::dma2_1:
            return DMA2_Stream1;
        case streams::dma2_2:
            return DMA2_Stream2;
        case streams::dma2_3:
            return DMA2_Stream3;
        case streams::dma2_4:
            return DMA2_Stream4;
        case streams::dma2_5:
            return DMA2_Stream5;
        case streams::dma2_6:
            return DMA2_Stream6;
        case streams::dma2_7:
            return DMA2_Stream7;
    }
}

//!
//! \brief Gets stream number of given DMA stream.
//!
template< streams dma_stream >
constexpr auto get_stream_number()
{
    return 0xff & static_cast<std::underlying_type_t<streams>>(dma_stream);
}

//!
//! \brief Gets error flag descriptor of DMA stream.
//!
template< streams dma_stream >
constexpr auto get_err_flag()
{
    constexpr auto stream_no = get_stream_number< dma_stream >();

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
    }
}

//!
//! \brief Gets half-transfer flag descriptor of DMA stream.
//!
template< streams dma_stream >
constexpr auto get_ht_flag()
{
    constexpr auto stream_no = get_stream_number< dma_stream >();

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
    }
}

//!
//! \brief Gets transfer complete flag descriptor using of DMA stream.
//!
template< streams dma_stream >
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
    }
}

//!
//! \brief Gets IRQ number of given DMA stream.
//!
template< streams dma_stream >
constexpr auto get_irqn()
{
    switch (dma_stream) {
        case streams::dma1_0:
            return DMA1_Stream0_IRQn;
        case streams::dma1_1:
            return DMA1_Stream1_IRQn;
        case streams::dma1_2:
            return DMA1_Stream2_IRQn;
        case streams::dma1_3:
            return DMA1_Stream3_IRQn;
        case streams::dma1_4:
            return DMA1_Stream4_IRQn;
        case streams::dma1_5:
            return DMA1_Stream5_IRQn;
        case streams::dma1_6:
            return DMA1_Stream6_IRQn;
        case streams::dma1_7:
            return DMA1_Stream7_IRQn;
        case streams::dma2_0:
            return DMA2_Stream0_IRQn;
        case streams::dma2_1:
            return DMA2_Stream1_IRQn;
        case streams::dma2_2:
            return DMA2_Stream2_IRQn;
        case streams::dma2_3:
            return DMA2_Stream3_IRQn;
        case streams::dma2_4:
            return DMA2_Stream4_IRQn;
        case streams::dma2_5:
            return DMA2_Stream5_IRQn;
        case streams::dma2_6:
            return DMA2_Stream6_IRQn;
        case streams::dma2_7:
            return DMA2_Stream7_IRQn;
    }
}

//!
//! \brief Gets reset and clock control descriptor associated with a given stream.
//!
template< streams dma_stream >
constexpr auto get_rcc()
{
    if ((static_cast<std::underlying_type_t<streams>>(dma_stream) & 0xf00) == 0x100) {
        return RCC_AHB1Periph_DMA1;
    } else {
        return RCC_AHB1Periph_DMA2;
    }

}

//!
//! \brief Gets half-transfer interrupt flag descriptor of given dma stream.
//!
template< streams dma_stream >
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
    }
}

//!
//! \brief Gets transfer complete interrupt flag descriptor of given dma stream.
//!
template< streams dma_stream >
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
    }
}

//!
//! \brief Gets transfer error interrupt flag descriptor of given dma stream.
//!
template< streams dma_stream >
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
    }
}

//!
//! \brief Initializes DMA peripherial.
//!
template< streams dma_stream >
static inline void init_rcc()
{
    // stm32f4 DMA is placed on AHB1 bus only
    RCC_AHB1PeriphClockCmd(get_rcc< dma_stream >(), ENABLE);
}

//!
//! \brief Enables IRQ for given DMA stream and interrupt sources.
//!
template< streams dma_stream, uint32_t flags >
static inline void enable_irq()
{
    // Enable interrupt sources
    DMA_ITConfig(get_stream< dma_stream >(), flags, ENABLE);
}

//!
//! \brief Disables IRQ for given DMA stream and interrupt sources.
//!
template< streams dma_stream, uint32_t flags >
static inline void disable_irq()
{
    // Enable interrupt sources
    DMA_ITConfig(get_stream< dma_stream >(), flags, DISABLE);
}

//!
//! \brief Subscribes to DMA IRQ associated with given DMA stream.
//!
template< streams dma_stream >
void subscribe_irq(const std::function< void () > &handler)
{
    constexpr auto dma_irqn = get_irqn< dma_stream >();

    // Prevent spurious interrupts from occurrence
    irq_manager::mask(dma_irqn);

    // Do not expose old, not yet handled interrupts
    irq_manager::clear(dma_irqn);

    // Subscribe, actually
    irq_manager::subscribe(dma_irqn, handler);

    // Go on
    // TODO: think about letting user to decide when to unmask interrupts
    irq_manager::unmask(dma_irqn);
}

} // namespace dma

} // namespace ecl

#endif
