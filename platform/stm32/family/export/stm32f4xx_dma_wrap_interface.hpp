#ifndef DMA_WRAP_INTERFACE_HPP_
#define DMA_WRAP_INTERFACE_HPP_

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

enum class dma_data_sz
{
    byte = DMA_MemoryDataSize_Byte,
    word = DMA_MemoryDataSize_Word,
    hword = DMA_MemoryDataSize_HalfWord,
};

enum class dma_mode
{
    normal = DMA_Mode_Normal,
    circular = DMA_Mode_Circular,
};

//------------------------------------------------------------------------------

template<dma_stream Stream, dma_channel Channel>
struct dma_wrap_priv;

template<dma_stream Stream, dma_channel Channel>
class dma_wrap
{
public:
    dma_wrap() = delete;
    dma_wrap(dma_wrap &) = delete;

    static constexpr auto get_irqn();

    static void init();

    template<dma_data_sz Size = dma_data_sz::byte, dma_mode Mode = dma_mode::normal>
    static void
    mem_to_periph(const uint8_t *src, size_t size, volatile uint16_t *periph);

    template<dma_data_sz Size = dma_data_sz::byte, dma_mode Mode = dma_mode::normal>
    static void
    mem_to_periph(uint16_t filler, size_t cnt, volatile uint16_t *periph);

    template<dma_data_sz Size = dma_data_sz::byte, dma_mode Mode = dma_mode::normal>
    static void
    periph_to_mem(volatile uint16_t *periph, uint8_t *dst, size_t size);

    // TODO: consider merging all routines below into `start()` and `stop()` methods
    template<bool EnableTC = true, bool EnableHT = false, bool EnableErr = false>
    static void enable_events();

    template<bool DisableTC = true, bool DisableHT = false, bool DisableErr = false>
    static void disable_events();

    static void enable();

    static void disable();

    static bool tc();

    static bool ht();

    static bool err();

    static void clear_tc();

    static void clear_ht();

    static void clear_err();

    static auto bytes_left();

private:
    using priv = dma_wrap_priv<Stream, Channel>;
};

} // namespace ecl

#endif //DMA_WRAP_INTERFACE_HPP_
