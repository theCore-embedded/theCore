//! \file
//! \brief STM32L1XX DMA wrappers implementation
//!
#ifndef PLATFORM_STM32F4XX_DMA_WRAP_
#define PLATFORM_STM32F4XX_DMA_WRAP_

#include <stm32l1xx_dma.h>
#include <stm32l1xx_rcc.h>

#include <functional>
#include <type_traits>

#include "stm32_dma_wrap_interface.hpp"

namespace ecl
{

//! DMA channels present, according to the RM.
//! \details High-order byte represents DMA number, low order - channel number.
//!
enum class dma_channel
{
    dma1_1 = 0x101,
    dma1_2 = 0x102,
    dma1_3 = 0x103,
    dma1_4 = 0x104,
    dma1_5 = 0x105,
    dma1_6 = 0x106,
    dma1_7 = 0x107,
    dma2_1 = 0x201,
    dma2_2 = 0x202,
    dma2_3 = 0x203,
    dma2_4 = 0x204,
    dma2_5 = 0x205,
};

template<class Derived>
constexpr auto dma_wrap_base<Derived>::get_irqn()
{
    constexpr auto channel = Derived::channel;

    switch (channel) {
        case dma_channel::dma1_1:
            return DMA1_Channel1_IRQn;
        case dma_channel::dma1_2:
            return DMA1_Channel2_IRQn;
        case dma_channel::dma1_3:
            return DMA1_Channel3_IRQn;
        case dma_channel::dma1_4:
            return DMA1_Channel4_IRQn;
        case dma_channel::dma1_5:
            return DMA1_Channel5_IRQn;
        case dma_channel::dma1_6:
            return DMA1_Channel6_IRQn;
        case dma_channel::dma1_7:
            return DMA1_Channel7_IRQn;
        case dma_channel::dma2_1:
            return DMA2_Channel1_IRQn;
        case dma_channel::dma2_2:
            return DMA2_Channel2_IRQn;
        case dma_channel::dma2_3:
            return DMA2_Channel3_IRQn;
        case dma_channel::dma2_4:
            return DMA2_Channel4_IRQn;
        case dma_channel::dma2_5:
            return DMA2_Channel5_IRQn;
    }
}

template<class Derived>
void dma_wrap_base<Derived>::init()
{
    RCC_AHBPeriphClockCmd(Derived::get_rcc(), ENABLE);
}

template<class Derived>
template<dma_data_sz Size, dma_mode Mode>
void dma_wrap_base<Derived>::mem_to_periph(const uint8_t *src,
                                           size_t size,
                                           volatile uint16_t *periph)
{
    constexpr auto channel   = Derived::get_spl_channel();
    constexpr auto mode      = static_cast<uint32_t>(Mode);
    constexpr auto data_size = static_cast<uint32_t>(Size);
    constexpr auto data_div  = Derived::get_size_div(data_size);

    DMA_InitTypeDef dma_init;

    dma_init.DMA_Mode       = mode;
    dma_init.DMA_BufferSize = size / data_div;
    dma_init.DMA_M2M        = DMA_M2M_Disable;

    dma_init.DMA_DIR      = DMA_DIR_PeripheralDST;
    dma_init.DMA_Priority = DMA_Priority_Low;

    dma_init.DMA_PeripheralBaseAddr = reinterpret_cast<uint32_t>(periph);
    dma_init.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    dma_init.DMA_PeripheralDataSize = data_size;

    dma_init.DMA_MemoryBaseAddr = reinterpret_cast<uint32_t>(src);
    dma_init.DMA_MemoryInc      = DMA_MemoryInc_Enable;
    dma_init.DMA_MemoryDataSize = data_size;

    DMA_Init(channel, &dma_init);
}

template<class Derived>
template<dma_data_sz Size, dma_mode Mode>
void dma_wrap_base<Derived>::mem_to_periph(uint16_t filler,
                                           size_t cnt,
                                           volatile uint16_t *periph)
{
    constexpr auto channel      = Derived::get_spl_channel();
    constexpr auto mode         = static_cast<uint32_t>(Mode);
    constexpr auto data_size    = static_cast<uint32_t>(Size);
    constexpr auto data_div     = Derived::get_size_div(data_size);
    static auto    local_filler = 0;

    local_filler = filler;

    DMA_InitTypeDef dma_init;

    dma_init.DMA_Mode       = mode;
    dma_init.DMA_BufferSize = cnt / data_div;
    dma_init.DMA_M2M        = DMA_M2M_Disable;

    dma_init.DMA_DIR      = DMA_DIR_PeripheralDST;
    dma_init.DMA_Priority = DMA_Priority_Low;

    dma_init.DMA_PeripheralBaseAddr = reinterpret_cast<uint32_t>(periph);
    dma_init.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    dma_init.DMA_PeripheralDataSize = data_size;

    dma_init.DMA_MemoryBaseAddr = reinterpret_cast<uint32_t>(&local_filler);
    dma_init.DMA_MemoryInc      = DMA_MemoryInc_Disable;
    dma_init.DMA_MemoryDataSize = data_size;

    DMA_Init(channel, &dma_init);
}

template<class Derived>
template<dma_data_sz Size, dma_mode Mode>
void dma_wrap_base<Derived>::periph_to_mem(volatile uint16_t *periph,
                                           uint8_t *dst,
                                           size_t size)
{
    constexpr auto channel   = Derived::get_spl_channel();
    constexpr auto mode      = static_cast<uint32_t>(Mode);
    constexpr auto data_size = static_cast<uint32_t>(Size);
    constexpr auto data_div  = Derived::get_size_div(data_size);

    DMA_InitTypeDef dma_init;

    dma_init.DMA_Mode       = mode;
    dma_init.DMA_BufferSize = size / data_div;
    dma_init.DMA_M2M        = DMA_M2M_Disable;

    dma_init.DMA_DIR      = DMA_DIR_PeripheralSRC;
    dma_init.DMA_Priority = DMA_Priority_Low;

    dma_init.DMA_PeripheralBaseAddr = reinterpret_cast<uint32_t>(periph);
    dma_init.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    dma_init.DMA_PeripheralDataSize = data_size;

    dma_init.DMA_MemoryBaseAddr = reinterpret_cast<uint32_t>(dst);
    dma_init.DMA_MemoryInc      = DMA_MemoryInc_Enable;
    dma_init.DMA_MemoryDataSize = data_size;

    DMA_Init(channel, &dma_init);
}

template<class Derived>
template<dma_data_sz Size, dma_mode Mode>
void
dma_wrap_base<Derived>::periph_to_mem(volatile uint16_t *periph, size_t size)
{
    constexpr auto channel    = Derived::get_spl_channel();
    constexpr auto mode       = static_cast<uint32_t>(Mode);
    constexpr auto data_size  = static_cast<uint32_t>(Size);
    constexpr auto data_div   = Derived::get_size_div(data_size);
    static auto    local_sink = 0;

    DMA_InitTypeDef dma_init;

    dma_init.DMA_Mode       = mode;
    dma_init.DMA_BufferSize = size / data_div;
    dma_init.DMA_M2M        = DMA_M2M_Disable;

    dma_init.DMA_DIR      = DMA_DIR_PeripheralSRC;
    dma_init.DMA_Priority = DMA_Priority_Low;

    dma_init.DMA_PeripheralBaseAddr = reinterpret_cast<uint32_t>(periph);
    dma_init.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    dma_init.DMA_PeripheralDataSize = data_size;

    dma_init.DMA_MemoryBaseAddr = reinterpret_cast<uint32_t>(&local_sink);
    dma_init.DMA_MemoryInc      = DMA_MemoryInc_Disable;
    dma_init.DMA_MemoryDataSize = data_size;

    DMA_Init(channel, &dma_init);
}

// TODO: consider merging all routines below into `start()` and `stop()` methods
template<class Derived>
template<bool EnableTC, bool EnableHT, bool EnableErr>
void dma_wrap_base<Derived>::enable_events()
{
    constexpr auto channel = Derived::get_spl_channel();
    constexpr auto flags   = (EnableTC ? DMA_IT_TC : 0)
                             | (EnableHT ? DMA_IT_HT : 0)
                             | (EnableErr ? DMA_IT_TE : 0);

    DMA_ITConfig(channel, flags, ENABLE);
}

template<class Derived>
template<bool DisableTC, bool DisableHT, bool DisableErr>
void dma_wrap_base<Derived>::disable_events()
{
    constexpr auto channel = Derived::get_spl_channel();
    constexpr auto flags   = (DisableTC ? DMA_IT_TC : 0)
                             | (DisableHT ? DMA_IT_HT : 0)
                             | (DisableErr ? DMA_IT_TE : 0);

    DMA_ITConfig(channel, flags, DISABLE);
}

template<class Derived>
void dma_wrap_base<Derived>::enable()
{
    constexpr auto channel = Derived::get_spl_channel();
    DMA_Cmd(channel, ENABLE);
}

template<class Derived>
void dma_wrap_base<Derived>::disable()
{
    constexpr auto channel = Derived::get_spl_channel();
    DMA_Cmd(channel, DISABLE);
}

template<class Derived>
bool dma_wrap_base<Derived>::tc()
{
    constexpr auto flag = Derived::get_tc_flag();
    return DMA_GetFlagStatus(flag) == SET;
}

template<class Derived>
bool dma_wrap_base<Derived>::ht()
{
    constexpr auto flag = Derived::get_ht_flag();
    return DMA_GetFlagStatus(flag) == SET;
}

template<class Derived>
bool dma_wrap_base<Derived>::err()
{
    constexpr auto flag = Derived::get_err_flag();
    return DMA_GetFlagStatus(flag) == SET;
}

template<class Derived>
void dma_wrap_base<Derived>::clear_tc()
{
    constexpr auto flag   = Derived::get_tc_flag();
    constexpr auto iflag  = Derived::get_tc_if();
    constexpr auto gflag  = Derived::get_global_flag();
    constexpr auto giflag = Derived::get_global_if();

    DMA_ClearFlag(flag);
    DMA_ClearITPendingBit(iflag);
    DMA_ClearFlag(gflag);
    DMA_ClearITPendingBit(giflag);

    Derived::handle_global_interrupt();
}

template<class Derived>
void dma_wrap_base<Derived>::clear_ht()
{
    constexpr auto flag  = Derived::get_ht_flag();
    constexpr auto iflag = Derived::get_ht_if();

    DMA_ClearFlag(flag);
    DMA_ClearITPendingBit(iflag);

    Derived::handle_global_interrupt();
}

template<class Derived>
void dma_wrap_base<Derived>::clear_err()
{
    constexpr auto flag  = Derived::get_err_flag();
    constexpr auto iflag = Derived::get_err_if();

    DMA_ClearFlag(flag);
    DMA_ClearITPendingBit(iflag);

    Derived::handle_global_interrupt();
}

template<class Derived>
auto dma_wrap_base<Derived>::bytes_left()
{
    constexpr auto channel    = Derived::get_spl_channel();
    auto           items_left = DMA_GetCurrDataCounter(channel);
    auto           reg        = channel->CCR & (0b11 << 10); // Fetch data size
    auto           div        = Derived::get_size_div(reg);

    return reg * div;
}

//------------------------------------------------------------------------------

template<dma_channel Channel>
struct dma_wrap : dma_wrap_base<dma_wrap<Channel>>
{
    static constexpr auto channel = Channel;

    constexpr static auto get_spl_channel();
    constexpr static auto get_size_div(uint32_t data_size);

    constexpr static auto get_rcc();

    constexpr static auto get_err_flag();
    constexpr static auto get_ht_flag();
    constexpr static auto get_tc_flag();
    constexpr static auto get_global_flag();

    constexpr static auto get_err_if();
    constexpr static auto get_ht_if();
    constexpr static auto get_tc_if();
    constexpr static auto get_global_if();

    constexpr static void handle_global_interrupt();
};

template<dma_channel Channel>
constexpr auto dma_wrap<Channel>::get_spl_channel()
{
    switch (Channel) {
        case dma_channel::dma1_1:
            return DMA1_Channel1;
        case dma_channel::dma1_2:
            return DMA1_Channel2;
        case dma_channel::dma1_3:
            return DMA1_Channel3;
        case dma_channel::dma1_4:
            return DMA1_Channel4;
        case dma_channel::dma1_5:
            return DMA1_Channel5;
        case dma_channel::dma1_6:
            return DMA1_Channel6;
        case dma_channel::dma1_7:
            return DMA1_Channel7;
        case dma_channel::dma2_1:
            return DMA2_Channel1;
        case dma_channel::dma2_2:
            return DMA2_Channel2;
        case dma_channel::dma2_3:
            return DMA2_Channel3;
        case dma_channel::dma2_4:
            return DMA2_Channel4;
        case dma_channel::dma2_5:
            return DMA2_Channel5;
    }
}

template<dma_channel Channel>
constexpr auto dma_wrap<Channel>::get_size_div(uint32_t data_size)
{
    // DMA_MemoryDataSize encodes data size in a specific way, so shift can be
    // applied and divider can be obtained.
    // However, this can be possibly changed in SPL.
    return data_size == DMA_MemoryDataSize_Byte ? 1 : data_size >> 9;
}

template<dma_channel Channel>
constexpr auto dma_wrap<Channel>::get_rcc()
{
    if ((static_cast<std::underlying_type_t <dma_channel>>(Channel) & 0xf00) ==
        0x100) {
        return RCC_AHBPeriph_DMA1;
    } else {
        return RCC_AHBPeriph_DMA2;
    }
}

template<dma_channel Channel>
constexpr auto dma_wrap<Channel>::get_err_flag()
{
    switch (Channel) {
        case dma_channel::dma1_1:
            return DMA1_FLAG_TE1;
        case dma_channel::dma1_2:
            return DMA1_FLAG_TE2;
        case dma_channel::dma1_3:
            return DMA1_FLAG_TE3;
        case dma_channel::dma1_4:
            return DMA1_FLAG_TE4;
        case dma_channel::dma1_5:
            return DMA1_FLAG_TE5;
        case dma_channel::dma1_6:
            return DMA1_FLAG_TE6;
        case dma_channel::dma1_7:
            return DMA1_FLAG_TE7;
        case dma_channel::dma2_1:
            return DMA2_FLAG_TE1;
        case dma_channel::dma2_2:
            return DMA2_FLAG_TE2;
        case dma_channel::dma2_3:
            return DMA2_FLAG_TE3;
        case dma_channel::dma2_4:
            return DMA2_FLAG_TE4;
        case dma_channel::dma2_5:
            return DMA2_FLAG_TE5;
    }
}

template<dma_channel Channel>
constexpr auto dma_wrap<Channel>::get_ht_flag()
{
    switch (Channel) {
        case dma_channel::dma1_1:
            return DMA1_FLAG_HT1;
        case dma_channel::dma1_2:
            return DMA1_FLAG_HT2;
        case dma_channel::dma1_3:
            return DMA1_FLAG_HT3;
        case dma_channel::dma1_4:
            return DMA1_FLAG_HT4;
        case dma_channel::dma1_5:
            return DMA1_FLAG_HT5;
        case dma_channel::dma1_6:
            return DMA1_FLAG_HT6;
        case dma_channel::dma1_7:
            return DMA1_FLAG_HT7;
        case dma_channel::dma2_1:
            return DMA2_FLAG_HT1;
        case dma_channel::dma2_2:
            return DMA2_FLAG_HT2;
        case dma_channel::dma2_3:
            return DMA2_FLAG_HT3;
        case dma_channel::dma2_4:
            return DMA2_FLAG_HT4;
        case dma_channel::dma2_5:
            return DMA2_FLAG_HT5;
    }
}

template<dma_channel Channel>
constexpr auto dma_wrap<Channel>::get_tc_flag()
{
    switch (Channel) {
        case dma_channel::dma1_1:
            return DMA1_FLAG_TC1;
        case dma_channel::dma1_2:
            return DMA1_FLAG_TC2;
        case dma_channel::dma1_3:
            return DMA1_FLAG_TC3;
        case dma_channel::dma1_4:
            return DMA1_FLAG_TC4;
        case dma_channel::dma1_5:
            return DMA1_FLAG_TC5;
        case dma_channel::dma1_6:
            return DMA1_FLAG_TC6;
        case dma_channel::dma1_7:
            return DMA1_FLAG_TC7;
        case dma_channel::dma2_1:
            return DMA2_FLAG_TC1;
        case dma_channel::dma2_2:
            return DMA2_FLAG_TC2;
        case dma_channel::dma2_3:
            return DMA2_FLAG_TC3;
        case dma_channel::dma2_4:
            return DMA2_FLAG_TC4;
        case dma_channel::dma2_5:
            return DMA2_FLAG_TC5;
    }
}

template<dma_channel Channel>
constexpr auto dma_wrap<Channel>::get_global_flag()
{
    switch (Channel) {
        case dma_channel::dma1_1:
            return DMA1_FLAG_GL1;
        case dma_channel::dma1_2:
            return DMA1_FLAG_GL2;
        case dma_channel::dma1_3:
            return DMA1_FLAG_GL3;
        case dma_channel::dma1_4:
            return DMA1_FLAG_GL4;
        case dma_channel::dma1_5:
            return DMA1_FLAG_GL5;
        case dma_channel::dma1_6:
            return DMA1_FLAG_GL6;
        case dma_channel::dma1_7:
            return DMA1_FLAG_GL7;
        case dma_channel::dma2_1:
            return DMA2_FLAG_GL1;
        case dma_channel::dma2_2:
            return DMA2_FLAG_GL2;
        case dma_channel::dma2_3:
            return DMA2_FLAG_GL3;
        case dma_channel::dma2_4:
            return DMA2_FLAG_GL4;
        case dma_channel::dma2_5:
            return DMA2_FLAG_GL5;
    }

}

template<dma_channel Channel>
constexpr auto dma_wrap<Channel>::get_ht_if()
{
    switch (Channel) {
        case dma_channel::dma1_1:
            return DMA1_IT_HT1;
        case dma_channel::dma1_2:
            return DMA1_IT_HT2;
        case dma_channel::dma1_3:
            return DMA1_IT_HT3;
        case dma_channel::dma1_4:
            return DMA1_IT_HT4;
        case dma_channel::dma1_5:
            return DMA1_IT_HT5;
        case dma_channel::dma1_6:
            return DMA1_IT_HT6;
        case dma_channel::dma1_7:
            return DMA1_IT_HT7;
        case dma_channel::dma2_1:
            return DMA2_IT_HT1;
        case dma_channel::dma2_2:
            return DMA2_IT_HT2;
        case dma_channel::dma2_3:
            return DMA2_IT_HT3;
        case dma_channel::dma2_4:
            return DMA2_IT_HT4;
        case dma_channel::dma2_5:
            return DMA2_IT_HT5;
    }
}

template<dma_channel Channel>
constexpr auto dma_wrap<Channel>::get_tc_if()
{
    switch (Channel) {
        case dma_channel::dma1_1:
            return DMA1_IT_TC1;
        case dma_channel::dma1_2:
            return DMA1_IT_TC2;
        case dma_channel::dma1_3:
            return DMA1_IT_TC3;
        case dma_channel::dma1_4:
            return DMA1_IT_TC4;
        case dma_channel::dma1_5:
            return DMA1_IT_TC5;
        case dma_channel::dma1_6:
            return DMA1_IT_TC6;
        case dma_channel::dma1_7:
            return DMA1_IT_TC7;
        case dma_channel::dma2_1:
            return DMA2_IT_TC1;
        case dma_channel::dma2_2:
            return DMA2_IT_TC2;
        case dma_channel::dma2_3:
            return DMA2_IT_TC3;
        case dma_channel::dma2_4:
            return DMA2_IT_TC4;
        case dma_channel::dma2_5:
            return DMA2_IT_TC5;
    }
}

template<dma_channel Channel>
constexpr auto dma_wrap<Channel>::get_err_if()
{
    switch (Channel) {
        case dma_channel::dma1_1:
            return DMA1_IT_TE1;
        case dma_channel::dma1_2:
            return DMA1_IT_TE2;
        case dma_channel::dma1_3:
            return DMA1_IT_TE3;
        case dma_channel::dma1_4:
            return DMA1_IT_TE4;
        case dma_channel::dma1_5:
            return DMA1_IT_TE5;
        case dma_channel::dma1_6:
            return DMA1_IT_TE6;
        case dma_channel::dma1_7:
            return DMA1_IT_TE7;
        case dma_channel::dma2_1:
            return DMA2_IT_TE1;
        case dma_channel::dma2_2:
            return DMA2_IT_TE2;
        case dma_channel::dma2_3:
            return DMA2_IT_TE3;
        case dma_channel::dma2_4:
            return DMA2_IT_TE4;
        case dma_channel::dma2_5:
            return DMA2_IT_TE5;
    }
}

template<dma_channel Channel>
constexpr auto dma_wrap<Channel>::get_global_if()
{
    switch (Channel) {
        case dma_channel::dma1_1:
            return DMA1_IT_GL1;
        case dma_channel::dma1_2:
            return DMA1_IT_GL2;
        case dma_channel::dma1_3:
            return DMA1_IT_GL3;
        case dma_channel::dma1_4:
            return DMA1_IT_GL4;
        case dma_channel::dma1_5:
            return DMA1_IT_GL5;
        case dma_channel::dma1_6:
            return DMA1_IT_GL6;
        case dma_channel::dma1_7:
            return DMA1_IT_GL7;
        case dma_channel::dma2_1:
            return DMA2_IT_GL1;
        case dma_channel::dma2_2:
            return DMA2_IT_GL2;
        case dma_channel::dma2_3:
            return DMA2_IT_GL3;
        case dma_channel::dma2_4:
            return DMA2_IT_GL4;
        case dma_channel::dma2_5:
            return DMA2_IT_GL5;
    }
}

template<dma_channel Channel>
constexpr void dma_wrap<Channel>::handle_global_interrupt()
{
#if 0
    constexpr auto err_flag   = get_err_flag();
    constexpr auto err_iflag  = get_err_if();
    constexpr auto tc_flag    = get_tc_flag();
    constexpr auto tc_iflag   = get_tc_if();
    constexpr auto ht_flag    = get_ht_flag();
    constexpr auto ht_iflag   = get_ht_if();
#endif
}

} // namespace ecl

#endif // PLATFORM_STM32F4XX_DMA_WRAP_
