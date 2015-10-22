#ifndef PLATFORM_SPI_DEVICE_HPP
#define PLATFORM_SPI_DEVICE_HPP

#include <stm32f4xx_spi.h>
#include <sys/spi_cfgs.h>
#include <platform/irq_manager.hpp>
#include <type_traits>


// SPI platform configuration class
template< SPI_com_type  SPI_type,
          uint16_t      SPI_direction,
          uint16_t      SPI_mode,
          uint16_t      SPI_CPOL,
          uint16_t      SPI_CPHA,
          uint16_t      SPI_NSS,
          uint16_t      SPI_first_bit,
          uint32_t      SPI_clock
          >
struct SPI_config
{
    static constexpr SPI_InitTypeDef m_init_obj = {
        SPI_direction,
        SPI_mode,
        SPI_DataSize_8b,
        SPI_CPOL,
        SPI_CPHA,
        SPI_NSS,
        SPI_BaudRatePrescaler_8, // Initial prescaler
        SPI_first_bit,
        7
    };

    static constexpr uint32_t       m_clock    = SPI_clock;
    static constexpr SPI_com_type   m_type     = SPI_type;
};


// TODO: add code that checks if driver is in bidir mode or not
template< SPI_device   SPIx,            // TODO: move it to config
          class        SPI_config,
          class        DMA_TX,
          class        DMA_RX >
class SPI_dev
{
public:

    // General type to hold status bits
    using s_t = uint16_t;

    // DMA aliases
    using DMA_TX_t = DMA_TX;
    using DMA_RX_t = DMA_RX;

    // Type of SPI transaction
    static constexpr SPI_com_type com_type = SPI_config::m_type;

    // Status flags, supported by the device
    struct flags
    {
        static constexpr s_t empty  =   0;                      // No state
        static constexpr s_t ERR    =
                SPI_FLAG_CRCERR
                | SPI_FLAG_MODF
                | SPI_I2S_FLAG_OVR
                | SPI_I2S_FLAG_TIFRFE;                          // Simple error
        static constexpr s_t BSY    =   SPI_I2S_FLAG_BSY;       // Device is busy
        static constexpr s_t TX_RDY =   SPI_I2S_FLAG_TXE;       // Transmit line is ready
        static constexpr s_t RX_PND =   SPI_I2S_FLAG_RXNE;		// Receive pending

    };

    // TODO: substitude it
    SPI_dev();
    ~SPI_dev();

    // Lazy initialization, -1 if error. 0 otherwise
    int init();

    // -1 if error, 0 otherwise
    int open();
    // -1 if error, 0 otherwise
    int close();

    // -1 if error, [0, count] otherwise
    template< SPI_com_type mode = SPI_config::m_type,
              typename std::enable_if< mode == SPI_com_type::IRQ, int >::type = 0 >
    ssize_t write(const uint8_t *data, size_t count);

    // -1 if error, [0, count] otherwise
    template< SPI_com_type mode = SPI_config::m_type,
              typename std::enable_if< mode == SPI_com_type::IRQ, int >::type = 0 >
    ssize_t read(uint8_t *data, size_t count);

    // -1 if error, 0 otherwise
    template< SPI_com_type mode = SPI_config::m_type,
              typename std::enable_if< mode == SPI_com_type::DMA, int >::type = 0 >
    int write(DMA_TX &dma);

    // -1 if error, 0 otherwise
    template< SPI_com_type mode = SPI_config::m_type,
              typename std::enable_if< mode == SPI_com_type::DMA, int >::type = 0 >
    int read(DMA_RX &dma);

    // Queries peripheral status
    // INVALID if method failed, anything else from flags otherwise
    auto get_status() const;

    // Set of routines to work with IRQs
    // -1 if error, 0 otherwise
    int register_IRQ(const std::function< void() > &handler);

    // Get current IRQ status
    // INVALID if error, 0 otherwise
    auto get_IRQ() const;

    // Masks or unmasks all interrupts, associated with a device
    // -1 if error, 0 otherwise
    // TODO: drop it?
    int mask_IRQ();
    // TODO: merge it with clear_IRQ?
    int unmask_IRQ();

    // Clears pending IRQs
    // -1 if error, 0 otherwise
    int clear_IRQ();

    // -1 if error, 0 otherwise
    int deregister_IRQ();

private:
    static constexpr auto pick_SPI();
    static constexpr auto pick_RCC();
    static constexpr auto pick_RCC_fn();
    static constexpr auto pick_IT();

    // Depends on the evironment
    static auto pick_PCLK();

    // TODO: flags instead of two separate fields
    // Indicates that SPI is ready for use
    bool m_inited = false;
    // Flag to prevent multiple opening
    bool m_opened = false;
};

//------------------------------------------------------------------------------
// Interface

template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::SPI_dev()
{
}

template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::~SPI_dev()
{
    // TODO
    if (m_inited) {

    }
}


// TODO: implement, comments
template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
int SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::init()
{
    constexpr auto spi               = pick_SPI();
    constexpr auto RCC_Periph        = pick_RCC();
    constexpr auto RCC_fn            = pick_RCC_fn();

    // Why it does work in so strange way?
    constexpr auto init_const_obj = SPI_config::m_init_obj;
    auto init_obj = init_const_obj;

    // Current clock on corresponding bus
    auto APB_CLK = pick_PCLK();

    // To calculate closest supported clock:
    uint16_t quotient = APB_CLK / SPI_config::m_clock;

    // Prescaler has a range from 2 to 256
    if (quotient < 2) {
        quotient = 2;
    } else if (quotient > 256) {
        quotient = 256;
    }

    // Divider values maps to prescaler according to binary logarithm in following way:
    // Quo   Log2   Prescaler
    // 2     1      0   (0b00)
    // 4     2      1   (0b01)
    // 8     3      2   (0b10)
    // 16    4      3   (0b11)
    // So conversion formula will look like:
    // prescaler = log2(divider) - 1;
    // Using clz() is more efficient way to do it and covers case when quotient is not
    // power of two value.
    uint16_t presc = ((32 - 1 - __builtin_clz(quotient)) - 1) << 3;
    init_obj.SPI_BaudRatePrescaler = presc;

    RCC_fn(RCC_Periph, ENABLE);

    m_inited = true;

    SPI_Init(spi, &init_obj);

    return 0;
}

// TODO: implement, comments
template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
int SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::open()
{
    if (!m_inited)
        return -1;

    constexpr auto spi = pick_SPI();

    SPI_Cmd(spi, ENABLE);
    m_opened = true;
    return 0;
}

// TODO: implement, comments
template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
int SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::close()
{
    if (!m_opened)
        return -1;

    constexpr auto spi = pick_SPI();

    SPI_Cmd(spi, DISABLE);
    m_opened = false;
    return 0;
}

// IRQ mode --------------------------------------------------------------------
// God save us all...
template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
template< SPI_com_type mode,
          typename std::enable_if< mode == SPI_com_type::IRQ, int >::type >
ssize_t SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::write(const uint8_t *data, size_t count)
{
    if (!m_opened)
        return -1;

    if (!count)
        return 0;

    constexpr auto spi = pick_SPI();

    if (SPI_config::m_type != SPI_com_type::poll) {
        if (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET
                || SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_BSY) == SET) {
            return -2; // Device busy
        }
    } else {
        while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET) {}
    }

    SPI_I2S_SendData(spi, data[0]);

    // Wait until data will be transmitted
    if (SPI_config::m_type == SPI_com_type::poll) {
        while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET) { }
        while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_BSY) == SET) { }
    }

    return 1;
}

template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
template< SPI_com_type mode,
          typename std::enable_if< mode == SPI_com_type::IRQ, int >::type >
ssize_t SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::read(uint8_t *data, size_t count)
{
    if (!m_opened)
        return -1;

    if (!count)
        return 0;

    constexpr auto spi = pick_SPI();

    // TODO: drop it to separate template member!!!
    if (SPI_config::m_type != SPI_com_type::poll) {
        // TODO: busy not ok, if receive buffer is empty
        if (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET
                || SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_BSY) == SET) {
            return -2; // Device busy
        }
    } else {
        while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET) { }
    }

    data[0] = SPI_I2S_ReceiveData(spi);

    // TODO: wait until transaction is over

    return 1;
}

// DMA mode --------------------------------------------------------------------

template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
template< SPI_com_type mode,
          typename std::enable_if< mode == SPI_com_type::DMA, int >::type >
int SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::write(DMA_TX &dma)
{
    constexpr auto spi = pick_SPI();

    dma.set_destination(DMA_TX::role::periphery,
                        reinterpret_cast< volatile uint8_t *> (&spi->DR),
                        1);
    dma.submit();

    SPI_I2S_DMACmd(spi, SPI_I2S_DMAReq_Tx, ENABLE);

    return 0;
}

template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
template< SPI_com_type mode,
          typename std::enable_if< mode == SPI_com_type::DMA, int >::type >
int SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::read(DMA_RX &dma)
{
    constexpr auto spi = pick_SPI();
    dma.set_origin(DMA_RX::role::periphery,
                   reinterpret_cast< volatile const uint8_t *> (&spi->DR),
                   1);
    dma.submit();

    SPI_I2S_DMACmd(spi, SPI_I2S_DMAReq_Rx, ENABLE);

    return 0;
}


// TODO: implement, comments
template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
auto SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::get_status() const
{
    constexpr auto spi = pick_SPI();

    return spi->SR;
}

// TODO: implement, comments
template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
int SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::register_IRQ(const std::function< void()> &handler)
{
    constexpr auto spi = pick_SPI();
    // There is one global ISR for all SPI interrupts
    constexpr auto IRQn = pick_IT();

    // TODO: make it customizable
    SPI_I2S_ITConfig(spi, SPI_I2S_IT_ERR, ENABLE);
    SPI_I2S_ITConfig(spi, SPI_I2S_IT_TXE, ENABLE);
    SPI_I2S_ITConfig(spi, SPI_I2S_IT_RXNE, ENABLE);

    return IRQ_manager::subscribe(IRQn, handler);
}

// TODO: implement, comments
template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
auto SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::get_IRQ() const
{
    return this->get_status();
}

// TODO: implement, comments
template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
int SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::mask_IRQ()
{
    // There is one global ISR for all SPI interrupts
    constexpr auto IRQn = pick_IT();

    return IRQ_manager::mask(IRQn);
}

// TODO: implement, comments
template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
int SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::unmask_IRQ()
{
    // There is one global ISR for all SPI interrupts
    constexpr auto IRQn = pick_IT();

    return IRQ_manager::unmask(IRQn);
}

// TODO: implement, comments
template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
int SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::clear_IRQ()
{
    // TODO: improve error check
    // There is one global ISR for all SPI interrupts
    constexpr auto IRQn = pick_IT();
    return IRQ_manager::clear(IRQn);
}


//------------------------------------------------------------------------------
// Private section

// TODO: implement
template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
constexpr auto SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::pick_SPI()
{
    switch (SPIx) {
    case SPI_device::bus_1:
        return SPI1;
    case SPI_device::bus_2:
        return SPI2;
    case SPI_device::bus_3:
        return SPI3;
    case SPI_device::bus_4:
        return SPI4;
    case SPI_device::bus_5:
        return SPI5;
    case SPI_device::bus_6:
        return SPI6;
    default:
        // TODO: clarfy
        return static_cast< decltype(SPI1) >(nullptr);
    }
}

// TODO: implement
template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
constexpr auto SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::pick_RCC()
{
    // TODO: comments
    switch (SPIx) {
    case SPI_device::bus_1:
        return RCC_APB2Periph_SPI1;
    case SPI_device::bus_2:
        return RCC_APB1Periph_SPI2;
    case SPI_device::bus_3:
        return RCC_APB1Periph_SPI3;
    case SPI_device::bus_4:
        return RCC_APB2Periph_SPI4;
    case SPI_device::bus_5:
        return RCC_APB2Periph_SPI5;
    case SPI_device::bus_6:
        return RCC_APB2Periph_SPI6;
    default:
        // TODO: clarify
        return static_cast< decltype(RCC_APB2Periph_SPI6) >(-1);
    }
}

// TODO: implement
template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
constexpr auto SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::pick_RCC_fn()
{
    // APB1 - SPI3 SPI2
    // APB2 - SPI5 SPI6 SPI1 SPI4
    switch (SPIx) {
    case SPI_device::bus_1:
    case SPI_device::bus_5:
    case SPI_device::bus_4:
    case SPI_device::bus_6:
        return RCC_APB2PeriphClockCmd;
    case SPI_device::bus_2:
    case SPI_device::bus_3:
        return RCC_APB1PeriphClockCmd;
    default:
        // TODO: clarify
        return static_cast< decltype(&RCC_APB2PeriphClockCmd) >(nullptr);
    }
}

template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
constexpr auto SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::pick_IT()
{
    switch (SPIx) {
    case SPI_device::bus_1:
        return SPI1_IRQn;
    case SPI_device::bus_2:
        return SPI2_IRQn;
    case SPI_device::bus_3:
        return SPI3_IRQn;

        // TODO: these are not avaliable for target processor.
        // Need to be protected by ifdefs
#if 0
    case SPI_device::bus_4:
        return SPI4_IRQn;
    case SPI_device::bus_5:
        return SPI5_IRQn;
    case SPI_device::bus_6:
        return SPI6_IRQn;
    case SPI_device::bus_7:
        return SPI7_IRQn;
#endif
    default:
        // TODO: clarify
        return static_cast< IRQn_Type >(-1);
    }
}


template< SPI_device SPIx, class SPI_config, class DMA_TX, class DMA_RX >
auto SPI_dev< SPIx, SPI_config, DMA_TX, DMA_RX >::pick_PCLK()
{
    RCC_ClocksTypeDef clkcfg;
    RCC_GetClocksFreq(&clkcfg);

    switch (SPIx) {
    case SPI_device::bus_1:
    case SPI_device::bus_5:
    case SPI_device::bus_4:
    case SPI_device::bus_6:
        return clkcfg.PCLK2_Frequency;
    case SPI_device::bus_2:
    case SPI_device::bus_3:
        return clkcfg.PCLK1_Frequency;
    default:
        // TODO: clarify
        return static_cast< decltype(clkcfg.PCLK1_Frequency) >(-1);
    }

}
#endif // PLATFORM_SPI_DEVICE_H
