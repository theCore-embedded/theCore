//! \file
//! \brief STM32F4XX DMA wrappers implementation
//!
#ifndef PLATFORM_STM32F4XX_DMA_WRAP_
#define PLATFORM_STM32F4XX_DMA_WRAP_

#include <stm32f4xx_dma.h>
#include <stm32f4xx_rcc.h>

#include <functional>
#include <type_traits>

#include "stm32_dma_wrap_interface.hpp"

namespace ecl
{

//! DMA streams present, according to the RM.
//! \details High-order byte represents DMA number, low order - stream number
//!
enum class dma_stream
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

enum class dma_channel
{
    ch0 = DMA_Channel_0,
    ch1 = DMA_Channel_1,
    ch2 = DMA_Channel_2,
    ch3 = DMA_Channel_3,
    ch4 = DMA_Channel_4,
    ch5 = DMA_Channel_5,
    ch6 = DMA_Channel_6,
    ch7 = DMA_Channel_7,
};

template<class Impl>
constexpr auto dma_wrap_base<Impl>::get_irqn()
{
    switch (Impl::stream) {
        case dma_stream::dma1_0:
            return DMA1_Stream0_IRQn;
        case dma_stream::dma1_1:
            return DMA1_Stream1_IRQn;
        case dma_stream::dma1_2:
            return DMA1_Stream2_IRQn;
        case dma_stream::dma1_3:
            return DMA1_Stream3_IRQn;
        case dma_stream::dma1_4:
            return DMA1_Stream4_IRQn;
        case dma_stream::dma1_5:
            return DMA1_Stream5_IRQn;
        case dma_stream::dma1_6:
            return DMA1_Stream6_IRQn;
        case dma_stream::dma1_7:
            return DMA1_Stream7_IRQn;
        case dma_stream::dma2_0:
            return DMA2_Stream0_IRQn;
        case dma_stream::dma2_1:
            return DMA2_Stream1_IRQn;
        case dma_stream::dma2_2:
            return DMA2_Stream2_IRQn;
        case dma_stream::dma2_3:
            return DMA2_Stream3_IRQn;
        case dma_stream::dma2_4:
            return DMA2_Stream4_IRQn;
        case dma_stream::dma2_5:
            return DMA2_Stream5_IRQn;
        case dma_stream::dma2_6:
            return DMA2_Stream6_IRQn;
        case dma_stream::dma2_7:
            return DMA2_Stream7_IRQn;
    }
}

template<class Impl>
void dma_wrap_base<Impl>::init()
{
    RCC_AHB1PeriphClockCmd(Impl::get_rcc(), ENABLE);
}

template<class Impl>
template<dma_data_sz Size, dma_mode Mode>
void dma_wrap_base<Impl>::mem_to_periph(const uint8_t *src,
                                        size_t size,
                                        volatile uint16_t *periph)
{
    constexpr auto stream    = Impl::get_stream_ptr();
    constexpr auto channel   = static_cast<uint32_t>(Impl::channel);
    constexpr auto mode      = static_cast<uint32_t>(Mode);
    constexpr auto data_div  = Impl::get_size_div(Size);

    DMA_InitTypeDef dma_init;

    dma_init.DMA_Channel    = channel;
    dma_init.DMA_Mode       = mode;
    dma_init.DMA_BufferSize = size / data_div;

    dma_init.DMA_DIR             = DMA_DIR_MemoryToPeripheral;
    dma_init.DMA_Priority        = DMA_Priority_Low;
    dma_init.DMA_FIFOMode        = DMA_FIFOMode_Disable;
    dma_init.DMA_FIFOThreshold   = DMA_FIFOThreshold_1QuarterFull;
    dma_init.DMA_MemoryBurst     = DMA_MemoryBurst_Single;
    dma_init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    dma_init.DMA_PeripheralBaseAddr = reinterpret_cast<uint32_t>(periph);
    dma_init.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    dma_init.DMA_PeripheralDataSize = data_size_to_spl_periph(Size);

    dma_init.DMA_Memory0BaseAddr = reinterpret_cast<uint32_t>(src);
    dma_init.DMA_MemoryInc       = DMA_MemoryInc_Enable;
    dma_init.DMA_MemoryDataSize  = data_size_to_spl_memory(Size);

    DMA_Init(stream, &dma_init);
}

template<class Impl>
template<dma_data_sz Size, dma_mode Mode>
void dma_wrap_base<Impl>::mem_to_periph(uint16_t filler,
                                        size_t cnt,
                                        volatile uint16_t *periph)
{
    constexpr auto stream       = Impl::get_stream_ptr();
    constexpr auto channel      = static_cast<uint32_t>(Impl::channel);
    constexpr auto mode         = static_cast<uint32_t>(Mode);
    constexpr auto data_div     = Impl::get_size_div(Size);
    static auto    local_filler = filler;

    DMA_InitTypeDef dma_init;

    dma_init.DMA_Channel    = channel;
    dma_init.DMA_Mode       = mode;
    dma_init.DMA_BufferSize = cnt / data_div;

    dma_init.DMA_DIR             = DMA_DIR_MemoryToPeripheral;
    dma_init.DMA_Priority        = DMA_Priority_Low;
    dma_init.DMA_FIFOMode        = DMA_FIFOMode_Disable;
    dma_init.DMA_FIFOThreshold   = DMA_FIFOThreshold_1QuarterFull;
    dma_init.DMA_MemoryBurst     = DMA_MemoryBurst_Single;
    dma_init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    dma_init.DMA_PeripheralBaseAddr = reinterpret_cast<uint32_t>(periph);
    dma_init.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    dma_init.DMA_PeripheralDataSize = data_size_to_spl_periph(Size);

    dma_init.DMA_Memory0BaseAddr = reinterpret_cast<uint32_t>(&local_filler);
    dma_init.DMA_MemoryInc       = DMA_MemoryInc_Disable;
    dma_init.DMA_MemoryDataSize  = data_size_to_spl_memory(Size);

    DMA_Init(stream, &dma_init);
}

template<class Impl>
template<dma_data_sz Size, dma_mode Mode>
void dma_wrap_base<Impl>::periph_to_mem(volatile uint16_t *periph,
                                        uint8_t *dst,
                                        size_t size)
{
    constexpr auto stream    = Impl::get_stream_ptr();
    constexpr auto channel   = static_cast<uint32_t>(Impl::channel);
    constexpr auto mode      = static_cast<uint32_t>(Mode);
    constexpr auto data_div  = Impl::get_size_div(Size);

    DMA_InitTypeDef dma_init;

    dma_init.DMA_Channel    = channel;
    dma_init.DMA_Mode       = mode;
    dma_init.DMA_BufferSize = size / data_div;

    dma_init.DMA_DIR             = DMA_DIR_PeripheralToMemory;
    dma_init.DMA_Priority        = DMA_Priority_Low;
    dma_init.DMA_FIFOMode        = DMA_FIFOMode_Disable;
    dma_init.DMA_FIFOThreshold   = DMA_FIFOThreshold_1QuarterFull;
    dma_init.DMA_MemoryBurst     = DMA_MemoryBurst_Single;
    dma_init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    dma_init.DMA_PeripheralBaseAddr = reinterpret_cast<uint32_t>(periph);
    dma_init.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    dma_init.DMA_PeripheralDataSize = data_size_to_spl_periph(Size);

    dma_init.DMA_Memory0BaseAddr = reinterpret_cast<uint32_t>(dst);
    dma_init.DMA_MemoryInc       = DMA_MemoryInc_Enable;
    dma_init.DMA_MemoryDataSize  = data_size_to_spl_memory(Size);

    DMA_Init(stream, &dma_init);
}

template<class Impl>
template<dma_data_sz Size, dma_mode Mode>
void
dma_wrap_base<Impl>::periph_to_mem(volatile uint16_t *periph, size_t size)
{
    constexpr auto  stream     = Impl::get_stream_ptr();
    constexpr auto  channel    = static_cast<uint32_t>(Impl::channel);
    constexpr auto  mode       = static_cast<uint32_t>(Mode);
    constexpr auto  data_div   = Impl::get_size_div(Size);
    static uint32_t local_sink = 0;

    DMA_InitTypeDef dma_init;

    dma_init.DMA_Channel    = channel;
    dma_init.DMA_Mode       = mode;
    dma_init.DMA_BufferSize = size / data_div;

    dma_init.DMA_DIR             = DMA_DIR_PeripheralToMemory;
    dma_init.DMA_Priority        = DMA_Priority_Low;
    dma_init.DMA_FIFOMode        = DMA_FIFOMode_Disable;
    dma_init.DMA_FIFOThreshold   = DMA_FIFOThreshold_1QuarterFull;
    dma_init.DMA_MemoryBurst     = DMA_MemoryBurst_Single;
    dma_init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    dma_init.DMA_PeripheralBaseAddr = reinterpret_cast<uint32_t>(periph);
    dma_init.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    dma_init.DMA_PeripheralDataSize = data_size_to_spl_periph(Size);

    dma_init.DMA_Memory0BaseAddr = reinterpret_cast<uint32_t>(&local_sink);
    dma_init.DMA_MemoryInc       = DMA_MemoryInc_Disable;
    dma_init.DMA_MemoryDataSize  = data_size_to_spl_memory(Size);

    DMA_Init(stream, &dma_init);
}

// TODO: consider merging all routines below into `start()` and `stop()` methods
template<class Impl>
template<bool EnableTC, bool EnableHT, bool EnableErr>
void dma_wrap_base<Impl>::enable_events_irq()
{
    constexpr auto stream = Impl::get_stream_ptr();
    constexpr auto flags  = (EnableTC ? DMA_IT_TC : 0)
                            | (EnableHT ? DMA_IT_HT : 0)
                            | (EnableErr ? DMA_IT_TE : 0);

    DMA_ITConfig(stream, flags, ENABLE);
}

template<class Impl>
template<bool DisableTC, bool DisableHT, bool DisableErr>
void dma_wrap_base<Impl>::disable_events_irq()
{
    constexpr auto stream = Impl::get_stream_ptr();
    constexpr auto flags  = (DisableTC ? DMA_IT_TC : 0)
                            | (DisableHT ? DMA_IT_HT : 0)
                            | (DisableErr ? DMA_IT_TE : 0);

    DMA_ITConfig(stream, flags, DISABLE);
}

template<class Impl>
void dma_wrap_base<Impl>::enable()
{
    constexpr auto stream = Impl::get_stream_ptr();
    DMA_Cmd(stream, ENABLE);
}

template<class Impl>
void dma_wrap_base<Impl>::disable()
{
    constexpr auto stream = Impl::get_stream_ptr();
    DMA_Cmd(stream, DISABLE);
}

template<class Impl>
bool dma_wrap_base<Impl>::tc()
{
    constexpr auto stream = Impl::get_stream_ptr();
    constexpr auto flag   = Impl::get_tc_flag();
    return DMA_GetFlagStatus(stream, flag) == SET;
}

template<class Impl>
bool dma_wrap_base<Impl>::ht()
{
    constexpr auto stream = Impl::get_stream_ptr();
    constexpr auto flag   = Impl::get_ht_flag();
    return DMA_GetFlagStatus(stream, flag) == SET;
}

template<class Impl>
bool dma_wrap_base<Impl>::err()
{
    constexpr auto stream = Impl::get_stream_ptr();
    constexpr auto flag   = Impl::get_err_flag();
    return DMA_GetFlagStatus(stream, flag) == SET;
}

template<class Impl>
void dma_wrap_base<Impl>::clear_tc()
{
    constexpr auto stream = Impl::get_stream_ptr();
    constexpr auto flag   = Impl::get_tc_flag();
    constexpr auto iflag  = Impl::get_tc_if();

    DMA_ClearFlag(stream, flag);
    DMA_ClearITPendingBit(stream, iflag);
}

template<class Impl>
void dma_wrap_base<Impl>::clear_ht()
{
    constexpr auto stream = Impl::get_stream_ptr();
    constexpr auto flag   = Impl::get_ht_flag();
    constexpr auto iflag  = Impl::get_ht_if();

    DMA_ClearFlag(stream, flag);
    DMA_ClearITPendingBit(stream, iflag);
}

template<class Impl>
void dma_wrap_base<Impl>::clear_err()
{
    constexpr auto stream = Impl::get_stream_ptr();
    constexpr auto flag   = Impl::get_err_flag();
    constexpr auto iflag  = Impl::get_err_if();

    DMA_ClearFlag(stream, flag);
    DMA_ClearITPendingBit(stream, iflag);
}

template<class Impl>
auto dma_wrap_base<Impl>::bytes_left()
{
    constexpr auto stream     = Impl::get_stream_ptr();
    auto           reg        = stream->CR & 0x6000; // Fetch data size

    // DMA_MemoryDataSize encodes data size in a specific way, so shift can be
    // applied and divider can be obtained.
    // However, this can be possibly changed in SPL.
    auto div = reg == DMA_MemoryDataSize_Byte ? 1 : reg >> 12;

    return reg * div;
}

//------------------------------------------------------------------------------

//! DMA wrapper for STM32F4XX.
//! \tparam DMA stream, acoording to RM.
//! \tparam DMA channel, according to RM.
template<dma_stream Stream, dma_channel Channel>
struct dma_wrap : dma_wrap_base<dma_wrap<Stream, Channel>>
{
    constexpr static auto stream  = Stream;
    constexpr static auto channel = Channel;

    //! Gets a pointer to a stream DMA object, suitable for use with SPL.
    constexpr static auto get_stream_ptr();
    //! Gets stream DMA number, according to RM.
    constexpr static auto get_stream_number();
    //! Gets size divider for given data size bits from DMA control register.
	constexpr static auto get_size_div(dma_data_sz data_size);
    //! Gets RCC peripheral, associated with DMA.
    constexpr static auto get_rcc();

    //! Gets error flag of the DMA.
    constexpr static auto get_err_flag();
    //! Gets half-transfer flag of the DMA.
    constexpr static auto get_ht_flag();
    //! Gets transfer-complete flag of the DMA.
    constexpr static auto get_tc_flag();

    //! Gets error interrupt flag of the DMA.
    constexpr static auto get_err_if();
    //! Gets half-transfer interrupt flag of the DMA.
    constexpr static auto get_ht_if();
    //! Gets transfer-complete interrupt flag of the DMA.
    constexpr static auto get_tc_if();
};

template<dma_stream Stream, dma_channel Channel>
constexpr auto dma_wrap<Stream, Channel>::get_stream_ptr()
{
    switch (Stream) {
        case dma_stream::dma1_0:
            return DMA1_Stream0;
        case dma_stream::dma1_1:
            return DMA1_Stream1;
        case dma_stream::dma1_2:
            return DMA1_Stream2;
        case dma_stream::dma1_3:
            return DMA1_Stream3;
        case dma_stream::dma1_4:
            return DMA1_Stream4;
        case dma_stream::dma1_5:
            return DMA1_Stream5;
        case dma_stream::dma1_6:
            return DMA1_Stream6;
        case dma_stream::dma1_7:
            return DMA1_Stream7;
        case dma_stream::dma2_0:
            return DMA2_Stream0;
        case dma_stream::dma2_1:
            return DMA2_Stream1;
        case dma_stream::dma2_2:
            return DMA2_Stream2;
        case dma_stream::dma2_3:
            return DMA2_Stream3;
        case dma_stream::dma2_4:
            return DMA2_Stream4;
        case dma_stream::dma2_5:
            return DMA2_Stream5;
        case dma_stream::dma2_6:
            return DMA2_Stream6;
        case dma_stream::dma2_7:
            return DMA2_Stream7;
    }
}

template<dma_stream Stream, dma_channel Channel>
constexpr auto dma_wrap<Stream, Channel>::get_stream_number()
{
    return 0xff & static_cast<std::underlying_type_t <dma_stream>>(Stream);
}

template<dma_stream Stream, dma_channel Channel>
constexpr auto dma_wrap<Stream, Channel>::get_size_div(dma_data_sz data_size)
{
    switch (data_size) {
        case dma_data_sz::byte:
            return 1;
        case dma_data_sz::hword:
            return 2;
        case dma_data_sz::word:
            return 4;
    }
}

template<dma_stream Stream, dma_channel Channel>
constexpr auto dma_wrap<Stream, Channel>::get_rcc()
{
    if ((static_cast<std::underlying_type_t <dma_stream>>(Stream) & 0xf00) ==
        0x100) {
        return RCC_AHB1Periph_DMA1;
    } else {
        return RCC_AHB1Periph_DMA2;
    }
}

template<dma_stream Stream, dma_channel Channel>
constexpr auto dma_wrap<Stream, Channel>::get_err_flag()
{
    constexpr auto stream_no = get_stream_number();

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

template<dma_stream Stream, dma_channel Channel>
constexpr auto dma_wrap<Stream, Channel>::get_ht_flag()
{
    constexpr auto stream_no = get_stream_number();

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

template<dma_stream Stream, dma_channel Channel>
constexpr auto dma_wrap<Stream, Channel>::get_tc_flag()
{
    constexpr auto stream_no = get_stream_number();

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

template<dma_stream Stream, dma_channel Channel>
constexpr auto dma_wrap<Stream, Channel>::get_ht_if()
{
    constexpr auto stream_no = get_stream_number();

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

template<dma_stream Stream, dma_channel Channel>
constexpr auto dma_wrap<Stream, Channel>::get_tc_if()
{
    constexpr auto stream_no = get_stream_number();

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

template<dma_stream Stream, dma_channel Channel>
constexpr auto dma_wrap<Stream, Channel>::get_err_if()
{
    constexpr auto stream_no = get_stream_number();

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

} // namespace ecl

#endif // PLATFORM_STM32F4XX_DMA_WRAP_
