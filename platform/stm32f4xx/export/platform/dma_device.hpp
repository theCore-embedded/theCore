#ifndef PLATFORM_DMA_DEVICE
#define PLATFORM_DMA_DEVICE

#include <stm32f4xx_dma.h>
#include <functional>

// TODO: comments
template< std::uintptr_t DMA_stream, uint32_t channel >
class DMA_dev
{
public:
    DMA_dev();
    ~DMA_dev();

    // Type for status flags
    using s_t = uint32_t;

    // Parent type

    struct flags
    {
    private:
        using dma = DMA_dev< DMA_stream, channel >;

    public:
        static constexpr s_t HT     = dma::pick_HT();
        static constexpr s_t TC     = dma::pick_TC();
        static constexpr s_t ERR    = dma::pick_ERR();
    };

    // Role of transfer endpoints
    enum class role
    {
        memory      = 0,
        periphery   = 1
    };

    int set_origin(role r,
                   volatile const uint8_t* address,
                   size_t size,
                   bool increment = true);

    int set_destination(role r,
                        volatile uint8_t* address,
                        size_t size,
                        bool increment = true);

    // <<<<<<<<<<< MOVE IT OUT OF CLASS SCOPE
    size_t get_dest_size() const { return m_destination_size; }

    auto get_status() const;

    int submit();
    int complete();

    int enable_IRQ(const std::function< void() > &handler, s_t flags = -1);
    int complete_IRQ(s_t flags = -1);

private:
    static constexpr auto pick_stream();
    static constexpr auto pick_stream_no();
    static constexpr auto pick_RCC();
    static constexpr auto pick_HT();
    static constexpr auto pick_TC();
    static constexpr auto pick_ERR();

    // Interrupts
    static constexpr auto pick_IT();
    static constexpr auto pick_HT_IF();
    static constexpr auto pick_TC_IF();
    static constexpr auto pick_ERR_IF();

    // Enables IRQ right before DMA transaction start
    int enable_IRQ();

    std::pair< role, volatile const uint8_t* > m_origin;
    std::pair< role, volatile uint8_t* > m_destination;
    bool   m_origin_inc;
    size_t m_origin_size;
    bool   m_destination_inc;
    size_t m_destination_size;

    std::function< void () >    m_handler;
    s_t                         m_IT_flags;
};


template< std::uintptr_t DMA_stream, uint32_t channel  >
DMA_dev< DMA_stream, channel >::DMA_dev()
    :m_origin{ role::memory, nullptr }
    ,m_destination{ role::memory, nullptr }
    ,m_origin_size{ 0 }
    ,m_destination_size{ 0 }
{
    constexpr auto RCC_DMA = pick_RCC();
    RCC_AHB1PeriphClockCmd(RCC_DMA, ENABLE);
}

template< std::uintptr_t DMA_stream, uint32_t channel  >
DMA_dev< DMA_stream, channel >::~DMA_dev()
{
    // TODO: disable interrupts, disable DMA stream?
}

template< std::uintptr_t DMA_stream, uint32_t channel  >
int DMA_dev< DMA_stream, channel >::set_origin(role r,
                                               volatile const uint8_t* address,
                                               size_t size,
                                               bool increment)
{
    if (!address || !size)
        return -1;

    m_origin = { r, address };
    m_origin_size = size;
    m_origin_inc = increment;
    return 0;
}

template< std::uintptr_t DMA_stream, uint32_t channel  >
int DMA_dev< DMA_stream, channel >::set_destination(role r,
                                                    volatile uint8_t* address,
                                                    size_t size,
                                                    bool increment)
{
    if (!address || !size)
        return -1;

    m_destination = { r, address };
    m_destination_size = size;
    m_destination_inc = increment;
    return 0;
}

template< std::uintptr_t DMA_stream, uint32_t channel  >
auto DMA_dev< DMA_stream, channel >::get_status() const
{
    constexpr auto stream = pick_stream();
    s_t flags = 0;

    if (DMA_GetFlagStatus(stream, flags::HT) == SET) {
        flags |= flags::HT;
    }

    if (DMA_GetFlagStatus(stream, flags::TC) == SET) {
        flags |= flags::TC;
    }

    if (DMA_GetFlagStatus(stream, flags::ERR) == SET) {
        flags |= flags::ERR;
    }

    return flags;
}

template< std::uintptr_t DMA_stream, uint32_t channel  >
int DMA_dev< DMA_stream, channel >::submit()
{
    constexpr auto stream = pick_stream();
    DMA_Cmd(stream, DISABLE);

    DMA_InitTypeDef init;
    DMA_StructInit(&init);

    init.DMA_Channel = channel;

    // Encode direction
    if (m_destination.first == role::memory) {
        if (m_destination_size < m_origin_size) {
            // Provided buffer is not large enough
            return -1;
        }

        init.DMA_PeripheralBaseAddr = reinterpret_cast< uint32_t >(m_origin.second);
        init.DMA_Memory0BaseAddr    = reinterpret_cast< uint32_t >(m_destination.second);

        // Assume that transaction forced by destination
        init.DMA_BufferSize = m_destination_size;

        if (m_destination_inc) {
            init.DMA_MemoryInc = DMA_MemoryInc_Enable;
        }
        if (m_origin_inc) {
            init.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
        }

        if (m_origin.first == role::memory) {
            init.DMA_DIR = DMA_DIR_MemoryToMemory;
        } else {
            init.DMA_DIR = DMA_DIR_PeripheralToMemory;
            init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        }
    } else {
        if (m_origin_size < m_destination_size) {
            // Provided buffer is not large enough
            return -1;
        }

        init.DMA_BufferSize         = m_origin_size;
        init.DMA_Memory0BaseAddr    = reinterpret_cast< uint32_t >(m_origin.second);
        init.DMA_PeripheralBaseAddr = reinterpret_cast< uint32_t >(m_destination.second);
        init.DMA_DIR                = DMA_DIR_MemoryToPeripheral;

        if (m_destination_inc) {
            init.DMA_PeripheralInc  = DMA_PeripheralInc_Enable;
        }
        if (m_origin_inc) {
            init.DMA_MemoryInc      = DMA_MemoryInc_Enable;
        }

    }

    enable_IRQ();

    DMA_Init(stream, &init);
    DMA_Cmd(stream, ENABLE);

    return 0;
}

template< std::uintptr_t DMA_stream, uint32_t channel  >
int DMA_dev< DMA_stream, channel >::complete()
{
    constexpr auto DMA_IT = pick_IT();
    constexpr auto stream = pick_stream();

    DMA_Cmd(stream, DISABLE);
    IRQ_manager::mask(DMA_IT);
    IRQ_manager::unsubscribe(DMA_IT);

    // Clear all flags
    DMA_ClearFlag(stream, pick_TC() | pick_HT() | pick_ERR());
    // Clear all pending interrupts
    DMA_ClearITPendingBit(stream, pick_TC_IF() | pick_HT_IF() | pick_ERR_IF());
    // Disable stream completely
    DMA_DeInit(stream);

    return 0;
}


template< std::uintptr_t DMA_stream, uint32_t channel  >
int DMA_dev< DMA_stream, channel >::enable_IRQ(
        const std::function< void() > &handler, s_t flags)
{
    if (!flags)
        return -1;

    /* Required to enable IRQ right before DMA start */
    m_handler = handler;
    m_IT_flags = flags;


    return 0;
}

template< std::uintptr_t DMA_stream, uint32_t channel  >
int DMA_dev< DMA_stream, channel >::complete_IRQ(s_t flags)
{
    constexpr auto DMA_IT = pick_IT();
    constexpr auto stream = pick_stream();
    uint32_t DMA_IT_flags = 0;

    if ((flags & pick_HT()) == pick_HT()) {
        DMA_IT_flags |= pick_HT_IF();
    }

    if ((flags & pick_TC()) == pick_TC()) {
        DMA_IT_flags |= pick_TC_IF();
    }

    if ((flags & pick_ERR()) == pick_ERR()) {
        DMA_IT_flags |= pick_ERR_IF();
    }

    IRQ_manager::mask(DMA_IT);

    if (flags)
        DMA_ClearFlag(stream, flags);

    if (DMA_IT_flags)
        DMA_ClearITPendingBit(stream, DMA_IT_flags);

    IRQ_manager::clear(DMA_IT);
    IRQ_manager::unmask(DMA_IT);

    return 0;
}


// Private members -------------------------------------------------------------

template< std::uintptr_t DMA_stream, uint32_t channel  >
constexpr auto DMA_dev< DMA_stream, channel >::pick_stream()
{
    return reinterpret_cast< DMA_Stream_TypeDef* >(DMA_stream);
}

template< std::uintptr_t DMA_stream, uint32_t channel  >
constexpr auto DMA_dev< DMA_stream, channel >::pick_stream_no()
{
    constexpr auto stream = pick_stream();

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

template< std::uintptr_t DMA_stream, uint32_t channel  >
constexpr auto DMA_dev< DMA_stream, channel >::pick_RCC()
{
    constexpr auto stream = pick_stream();

    /* Reset interrupt pending bits for the selected stream */
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


template< std::uintptr_t DMA_stream, uint32_t channel  >
constexpr auto DMA_dev< DMA_stream, channel >::pick_HT_IF()
{
    constexpr auto stream_no = pick_stream_no();

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
        return 0xfful; // TODO
    }

}

template< std::uintptr_t DMA_stream, uint32_t channel >
constexpr auto DMA_dev< DMA_stream, channel >::pick_TC_IF()
{
    constexpr auto stream_no = pick_stream_no();

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

template< std::uintptr_t DMA_stream, uint32_t channel  >
constexpr auto DMA_dev< DMA_stream, channel >::pick_ERR_IF()
{
    constexpr auto stream_no = pick_stream_no();

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


template< std::uintptr_t DMA_stream, uint32_t channel  >
constexpr auto DMA_dev< DMA_stream, channel >::pick_HT()
{
    constexpr auto stream_no = pick_stream_no();

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

template< std::uintptr_t DMA_stream, uint32_t channel >
constexpr auto DMA_dev< DMA_stream, channel >::pick_TC()
{
    constexpr auto stream_no = pick_stream_no();

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

template< std::uintptr_t DMA_stream, uint32_t channel  >
constexpr auto DMA_dev< DMA_stream, channel >::pick_ERR()
{
    constexpr auto stream_no = pick_stream_no();

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


template< std::uintptr_t DMA_stream, uint32_t channel  >
constexpr auto DMA_dev< DMA_stream, channel >::pick_IT()
{
    constexpr auto stream = pick_stream();

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

template< std::uintptr_t DMA_stream, uint32_t channel >
int DMA_dev< DMA_stream, channel >::enable_IRQ()
{
    constexpr auto DMA_IT = pick_IT();
    constexpr auto stream = pick_stream();

    uint32_t DMA_IT_flags = 0;

    if ((m_IT_flags & pick_HT()) == pick_HT()) {
        DMA_IT_flags |= DMA_IT_HT;
    }

    if ((m_IT_flags & pick_TC()) == pick_TC()) {
        DMA_IT_flags |= DMA_IT_TC;
    }

    if ((m_IT_flags & pick_ERR()) == pick_ERR()) {
        DMA_IT_flags |= (DMA_IT_TE | DMA_IT_FE);
    }

    // Disable all interrupt sources before we go
    constexpr auto to_disable = DMA_IT_TE | DMA_IT_FE | DMA_IT_TC | DMA_IT_HT;

    DMA_ITConfig(stream, to_disable, DISABLE);

    // Enable all
    DMA_ITConfig(stream, DMA_IT_flags, ENABLE);

    IRQ_manager::mask(DMA_IT);
    IRQ_manager::clear(DMA_IT);
    IRQ_manager::subscribe(DMA_IT, m_handler);
    IRQ_manager::unmask(DMA_IT);

    return 0;
}


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
    constexpr auto stream_no = get_stream< dma_stream >();

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
