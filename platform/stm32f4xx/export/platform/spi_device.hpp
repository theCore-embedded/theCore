#ifndef PLATFORM_SPI_DEVICE_HPP
#define PLATFORM_SPI_DEVICE_HPP

#include <stm32f4xx_spi.h>
#include <sys/spi_cfgs.h>
#include <platform/irq_manager.hpp>

// TODO: add code that checks if driver is in bidir mode or not
template< SPI_device SPIx, SPI_com_type com_type = SPI_com_type::poll >
class SPI_dev
{
public:

    // General type to hold status bits
    using s_t = uint16_t;

    // Status flags, supported by the device
    struct flags
    {
        static constexpr s_t empty  =   0;                      // No state
        static constexpr s_t ERR    =
                SPI_FLAG_CRCERR
                | SPI_FLAG_MODF
                | SPI_I2S_FLAG_OVR
                | SPI_I2S_FLAG_TIFRFE;                              // Simple error
        static constexpr s_t BSY    =   SPI_I2S_FLAG_BSY;      // Device is busy
        static constexpr s_t TX_RDY =   SPI_I2S_FLAG_TXE;		// Transmit line is ready
        static constexpr s_t RX_PND =   SPI_I2S_FLAG_RXNE;		// Receive pending

    };

    // TODO: substitude it
    SPI_dev(SPI_direction   direction = SPI_direction::BIDIR,
            SPI_mode        mode      = SPI_mode::master,
            SPI_CPOL        CPOL      = SPI_CPOL::low,
            SPI_CPHA        CPHA      = SPI_CPHA::second_edge,
            SPI_NSS_type    nssType   = SPI_NSS_type::HW,
            SPI_bit_order   bitOrder  = SPI_bit_order::MSB,
            uint32_t        clock     = 1000);
    ~SPI_dev();

    // Lazy initialization, -1 if error. 0 otherwise
    int init();

    // -1 if error, 0 otherwise
    int open();
    // -1 if error, 0 otherwise
    int close();

    // -1 if error, [0, count] otherwise
    ssize_t write(const uint8_t *data, size_t count);

    // -1 if error, [0, count] otherwise
    ssize_t read(uint8_t *data, size_t count);

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

    static constexpr auto pick_direction(SPI_direction direction);
    static constexpr auto pick_mode(SPI_mode mode);
    static constexpr auto pickCPOL(SPI_CPOL CPOL);
    static constexpr auto pickCPHA(SPI_CPHA CPHA);
    static constexpr auto pickNSS(SPI_NSS_type type);
    static constexpr auto pick_bit_order(SPI_bit_order order);
    static constexpr auto pick_IT();

    // Depends on the evironment
    static auto pick_PCLK();

    // Init object is required to be non-constant
    // See SPI_Init routine for more info
    SPI_InitTypeDef       m_initObj;
    const uint32_t        m_clock;

    // TODO: flags instead of two separate fields
    // Indicates that SPI is ready for use
    bool m_inited = false;
    // Flag to prevent multiple opening
    bool m_opened = false;
};

//------------------------------------------------------------------------------
// Interface

template< SPI_device SPIx, SPI_com_type com_type >
SPI_dev< SPIx, com_type >::SPI_dev(SPI_direction   direction,
                                   SPI_mode        mode,
                                   SPI_CPOL        CPOL,
                                   SPI_CPHA        CPHA,
                                   SPI_NSS_type    nssType,
                                   SPI_bit_order   bitOrder,
                                   uint32_t        clock)
    :m_initObj{
         pick_direction(direction),
         pick_mode(mode),
         SPI_DataSize_8b, // TODO: carefully configure
         pickCPOL(CPOL),
         pickCPHA(CPHA),
         pickNSS(nssType),
         SPI_BaudRatePrescaler_8, // Default value
         pick_bit_order(bitOrder),
         7,
         }
    ,m_clock{clock}

{
}

template< SPI_device SPIx, SPI_com_type com_type >
SPI_dev< SPIx, com_type >::~SPI_dev()
{
    // TODO
    if (m_inited) {

    }
}


// TODO: implement, comments
template< SPI_device SPIx, SPI_com_type com_type >
int SPI_dev< SPIx, com_type >::init()
{
    constexpr auto spi               = pick_SPI();
    constexpr auto RCC_Periph        = pick_RCC();
    constexpr auto RCC_fn            = pick_RCC_fn();

    // Current clock on corresponding bus
    auto APB_CLK = pick_PCLK();

    // To calculate closest supported clock:
    // 1. find
    uint16_t divider = APB_CLK / m_clock;

    // Prescaler has a range from 2 to 256
    if (divider < 2) {
        divider = 2;
    } else if (divider > 256) {
        divider = 256;
    }

    // Divider values maps to prescaler according to binary logarithm in following way:
    // Div   Log2   Prescaler
    // 2     1      0   (0b00)
    // 4     2      1   (0b01)
    // 8     3      2   (0b10)
    // 16    4      3   (0b11)
    // So conversion formula will look like:
    // prescaler = log2(divider) - 1;
    // Using clz is more efficient way to do it and covers case when divider is not
    // power of two value.
    uint16_t presc = ((32 - 1 - __builtin_clz(divider)) - 1) << 3;
    m_initObj.SPI_BaudRatePrescaler = presc;

    RCC_fn(RCC_Periph, ENABLE);

    m_inited = true;

    SPI_Init(spi, &m_initObj);

    return 0;
}

// TODO: implement, comments
template< SPI_device SPIx, SPI_com_type com_type >
int SPI_dev< SPIx, com_type >::open()
{
    if (!m_inited)
        return -1;

    constexpr auto spi = pick_SPI();

    SPI_Cmd(spi, ENABLE);
    m_opened = true;
    return 0;
}

// TODO: implement, comments
template< SPI_device SPIx, SPI_com_type com_type >
int SPI_dev< SPIx, com_type >::close()
{
    if (!m_opened)
        return -1;

    constexpr auto spi = pick_SPI();

    SPI_Cmd(spi, DISABLE);
    m_opened = false;
    return 0;
}

// TODO: implement, comments
template< SPI_device SPIx, SPI_com_type com_type >
ssize_t SPI_dev< SPIx, com_type >::write(const uint8_t *data, size_t count)
{
    if (!m_opened)
        return -1;

    if (!count)
        return 0;

    constexpr auto spi = pick_SPI();

    if (com_type != SPI_com_type::poll) {
        if (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET
                || SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_BSY == SET)) {
            return -2; // Device busy
        }
    } else {
        while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET) {}
    }

    SPI_I2S_SendData(spi, data[0]);

    // Wait until data will be transmitted
    if (com_type == SPI_com_type::poll) {
        while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET) { }
        while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_BSY) == SET) { }
    }

    return 1;
}

// TODO: implement, comments
template< SPI_device SPIx, SPI_com_type com_type >
ssize_t SPI_dev< SPIx, com_type >::read(uint8_t *data, size_t count)
{
    if (!m_opened)
        return -1;

    if (!count)
        return 0;

    constexpr auto spi = pick_SPI();

    if (com_type != SPI_com_type::poll) {
        // TODO: busy not ok, if receive buffer is empty
        if (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET
                || SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_BSY == SET)) {
            return -2; // Device busy
        }
    } else {
        while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET) { }
    }

    data[0] = SPI_I2S_ReceiveData(spi);

    // TODO: wait until transaction is over

    return 1;
}


// TODO: implement, comments
template< SPI_device SPIx, SPI_com_type com_type >
auto SPI_dev<SPIx, com_type>::get_status() const
{
    constexpr auto spi = pick_SPI();

    return spi->SR;
}

// TODO: implement, comments
template< SPI_device SPIx, SPI_com_type com_type >
int SPI_dev< SPIx, com_type >::register_IRQ(const std::function< void()> &handler)
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
template< SPI_device SPIx, SPI_com_type com_type >
auto SPI_dev< SPIx, com_type >::get_IRQ() const
{
    return this->get_status();
}

// TODO: implement, comments
template< SPI_device SPIx, SPI_com_type com_type >
int SPI_dev< SPIx, com_type >::mask_IRQ()
{
    // There is one global ISR for all SPI interrupts
    constexpr auto IRQn = pick_IT();

    return IRQ_manager::mask(IRQn);
}

// TODO: implement, comments
template< SPI_device SPIx, SPI_com_type com_type >
int SPI_dev< SPIx, com_type >::unmask_IRQ()
{
    // There is one global ISR for all SPI interrupts
    constexpr auto IRQn = pick_IT();

    return IRQ_manager::unmask(IRQn);
}

// TODO: implement, comments
template< SPI_device SPIx, SPI_com_type com_type >
int SPI_dev< SPIx, com_type >::clear_IRQ()
{
    // TODO: improve error check
    // There is one global ISR for all SPI interrupts
    constexpr auto IRQn = pick_IT();
    return IRQ_manager::clear(IRQn);
}


//------------------------------------------------------------------------------
// Private section

// TODO: implement
template< SPI_device SPIx, SPI_com_type com_type >
constexpr auto SPI_dev< SPIx, com_type >::pick_SPI()
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
template< SPI_device SPIx, SPI_com_type com_type >
constexpr auto SPI_dev< SPIx, com_type >::pick_RCC()
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
template< SPI_device SPIx, SPI_com_type com_type >
constexpr auto SPI_dev< SPIx, com_type >::pick_RCC_fn()
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

// TODO: implement
template< SPI_device SPIx, SPI_com_type com_type >
constexpr auto SPI_dev< SPIx, com_type >::pick_direction(SPI_direction direction)
{

    // TODO: add default section
    switch (direction) {
    case SPI_direction::TX:
        return SPI_Direction_1Line_Tx;
    case SPI_direction::RX:
        return SPI_Direction_1Line_Rx;
    case SPI_direction::BIDIR:
        return SPI_Direction_2Lines_FullDuplex;
    default:
        // TODO: clarify
        return static_cast< decltype(SPI_Direction_1Line_Tx) >(-1);
    }
}

// TODO: implement
template< SPI_device SPIx, SPI_com_type com_type >
constexpr auto SPI_dev< SPIx, com_type >::pick_mode(SPI_mode mode)
{
    switch (mode) {
    case SPI_mode::master:
        return SPI_Mode_Master;
    case SPI_mode::slave:
        return SPI_Mode_Slave;
    default:
        // TODO: clarify
        return static_cast< decltype(SPI_Mode_Slave) >(-1);
    }
}

// TODO: implement
template< SPI_device SPIx, SPI_com_type com_type >
constexpr auto SPI_dev< SPIx, com_type >::pickCPOL(SPI_CPOL CPOL)
{
    switch (CPOL) {
    case SPI_CPOL::low:
        return SPI_CPOL_Low;
    case SPI_CPOL::high:
        return SPI_CPOL_High;
    default:
        // TODO: clarify
        return static_cast< decltype(SPI_CPOL_High) >(-1);
    }
}

// TODO: implement
template< SPI_device SPIx, SPI_com_type com_type >
constexpr auto SPI_dev< SPIx, com_type >::pickCPHA(SPI_CPHA CPHA)
{
    switch (CPHA) {
    case SPI_CPHA::first_edge:
        return SPI_CPHA_1Edge;
    case SPI_CPHA::second_edge:
        return SPI_CPHA_2Edge;
    default:
        // TODO: clarify
        return static_cast< decltype(SPI_CPHA_2Edge) >(-1);
    }
}

template< SPI_device SPIx, SPI_com_type com_type >
constexpr auto SPI_dev< SPIx, com_type >::pickNSS(SPI_NSS_type type)
{
    switch (type) {
    case SPI_NSS_type::HW:
        return SPI_NSS_Hard;
    case SPI_NSS_type::SW:
        return SPI_NSS_Soft;
    default:
        // TODO: clarify
        return static_cast< decltype(SPI_NSS_Soft) >(-1);
    }
}

template< SPI_device SPIx, SPI_com_type com_type >
constexpr auto SPI_dev< SPIx, com_type >::pick_bit_order(SPI_bit_order order)
{

    switch (order) {
    case SPI_bit_order::MSB:
        return SPI_FirstBit_MSB;
    case SPI_bit_order::LSB:
        return SPI_FirstBit_LSB;
    default:
        // TODO: clarify
        return static_cast< decltype(SPI_FirstBit_LSB) >(-1);
    }

}

template< SPI_device SPIx, SPI_com_type com_type >
constexpr auto SPI_dev< SPIx, com_type >::pick_IT()
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


template< SPI_device SPIx, SPI_com_type com_type >
auto SPI_dev< SPIx, com_type >::pick_PCLK()
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
