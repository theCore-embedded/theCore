#ifndef DMA_WRAP_INTERFACE_HPP_
#define DMA_WRAP_INTERFACE_HPP_

namespace ecl
{

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

template<class Derived>
class dma_wrap_base
{
public:
    dma_wrap_base() = delete;
    dma_wrap_base(dma_wrap_base &) = delete;

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

    template<dma_data_sz Size = dma_data_sz::byte, dma_mode Mode = dma_mode::normal>
    static void
    periph_to_mem(volatile uint16_t *periph, size_t size);

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
};

} // namespace ecl

#endif //DMA_WRAP_INTERFACE_HPP_
