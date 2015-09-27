#ifndef PLATFORM_SPI_DEVICE_HPP
#define PLATFORM_SPI_DEVICE_HPP

#include <stm32f4xx_spi.h>
#include <sys/spi_cfgs.h>

// TODO: add code that checks if driver is in bidir mode or not
template< SPI_device SPIx >
class SPI_dev
{
public:
    // TODO: default values
    SPI_dev(SPI_direction   direction = SPI_direction::BIDIR,
            SPI_mode        mode      = SPI_mode::master,
            SPI_CPOL        CPOL      = SPI_CPOL::low,
            SPI_CPHA        CPHA      = SPI_CPHA::second_edge,
            SPI_NSS_type    nssType   = SPI_NSS_type::HW,
            SPI_bit_order   bitOrder  = SPI_bit_order::MSB,
            uint32_t        clock     = 0); // Not used for now. TODO here
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

private:
    static constexpr auto pickSPI();
    static constexpr auto pickRCC();
    static constexpr auto pickRCC_fn();

    static constexpr auto pickDir(SPI_direction direction);
    static constexpr auto pickMode(SPI_mode mode);
    static constexpr auto pickCPOL(SPI_CPOL CPOL);
    static constexpr auto pickCPHA(SPI_CPHA CPHA);
    static constexpr auto pickNSS(SPI_NSS_type nssType);
    static constexpr auto pickFirstBit(SPI_bit_order bitOrder);

    // Init object is required to be non-constant
    // See SPI_Init routine for more info
    SPI_InitTypeDef m_initObj;
    // TODO: use it!
    const uint32_t        m_clock;

    // TODO: flags instead of two separate fields
    // Indicates that SPI is ready for use
    bool m_inited = false;
    // Flag to prevent multiple opening
    bool m_opened = false;
};

//------------------------------------------------------------------------------
// Interface

template< SPI_device SPIx >
SPI_dev< SPIx >::SPI_dev(SPI_direction   direction,
                         SPI_mode        mode,
                         SPI_CPOL        CPOL,
                         SPI_CPHA        CPHA,
                         SPI_NSS_type     nssType,
                         SPI_bit_order    bitOrder,
                         uint32_t       clock)
    :m_initObj{
         pickDir(direction),
         pickMode(mode),
         SPI_DataSize_8b, // TODO: carefully configure
         pickCPOL(CPOL),
         pickCPHA(CPHA),
         pickNSS(nssType),
         SPI_BaudRatePrescaler_64, // TODO: clarify
         pickFirstBit(bitOrder),
         7,
         }
    ,m_clock(clock)

{
}

template< SPI_device SPIx >
SPI_dev< SPIx >::~SPI_dev()
{
    // TODO
    if (m_inited) {

    }
}


// TODO: implement, comments
template< SPI_device SPIx >
int SPI_dev< SPIx >::init()
{
    constexpr auto spi               = pickSPI();
    constexpr auto RCC_Periph        = pickRCC();
    constexpr auto RCC_fn            = pickRCC_fn();

    RCC_fn(RCC_Periph, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    m_inited = true;

    SPI_Init(spi, &m_initObj);

    return 0;
}

// TODO: implement, comments
template< SPI_device SPIx >
int SPI_dev< SPIx >::open()
{
    if (!m_inited)
        return -1;

    constexpr auto spi = pickSPI();

    SPI_Cmd(spi, ENABLE);
    m_opened = true;
    return 0;
}

// TODO: implement, comments
template< SPI_device SPIx >
int SPI_dev< SPIx >::close()
{
    if (!m_opened)
        return -1;

    constexpr auto spi = pickSPI();

    SPI_Cmd(spi, DISABLE);
    m_opened = false;
    return 0;
}

// TODO: implement, comments
template< SPI_device SPIx >
ssize_t SPI_dev< SPIx >::write(const uint8_t *data, size_t count)
{
    if (!m_opened)
        return -1;

    if (!count)
        return 0;

    constexpr auto spi = pickSPI();

    while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET) { }

    SPI_I2S_SendData(spi, data[0]);

    // Wait until data will be transmitted
    while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET) { }
    while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_BSY) == SET) { }

    return 1;
}

// TODO: implement, comments
template< SPI_device SPIx >
ssize_t SPI_dev< SPIx >::read(uint8_t *data, size_t count)
{
    if (!m_opened)
        return -1;

    if (!count)
        return 0;

    constexpr auto spi = pickSPI();

    while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET) { }

    data[0] = SPI_I2S_ReceiveData(spi);

    return 1;
}

//------------------------------------------------------------------------------
// Private section

// TODO: implement
template< SPI_device SPIx >
constexpr auto SPI_dev< SPIx >::pickSPI()
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
template< SPI_device SPIx >
constexpr auto SPI_dev< SPIx >::pickRCC()
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
template< SPI_device SPIx >
constexpr auto SPI_dev< SPIx >::pickRCC_fn()
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
template< SPI_device SPIx >
constexpr auto SPI_dev< SPIx >::pickDir(SPI_direction direction)
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
template< SPI_device SPIx >
constexpr auto SPI_dev< SPIx >::pickMode(SPI_mode mode)
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
template< SPI_device SPIx >
constexpr auto SPI_dev< SPIx >::pickCPOL(SPI_CPOL CPOL)
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
template< SPI_device SPIx >
constexpr auto SPI_dev< SPIx >::pickCPHA(SPI_CPHA CPHA)
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

template< SPI_device SPIx >
constexpr auto SPI_dev< SPIx >::pickNSS(SPI_NSS_type nssType)
{
    switch (nssType) {
    case SPI_NSS_type::HW:
        return SPI_NSS_Hard;
    case SPI_NSS_type::SW:
        return SPI_NSS_Soft;
    default:
        // TODO: clarify
        return static_cast< decltype(SPI_NSS_Soft) >(-1);
    }
}

template< SPI_device SPIx >
constexpr auto SPI_dev< SPIx >::pickFirstBit(SPI_bit_order bitOrder)
{
    switch (bitOrder) {
    case SPI_bit_order::MSB:
        return SPI_FirstBit_MSB;
    case SPI_bit_order::LSB:
        return SPI_FirstBit_LSB;
    default:
        // TODO: clarify
        return static_cast< decltype(SPI_FirstBit_LSB) >(-1);
    }
}



#endif // PLATFORM_SPI_DEVICE_H
