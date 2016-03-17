#ifndef PLATFORM_SPI_BUS_HPP_
#define PLATFORM_SPI_BUS_HPP_

#include <common/spi.hpp>
#include <platform/common/bus.hpp>
#include <platform/irq_manager.hpp>
#include <platform/dma_device.hpp>

#include <sys/types.h>

#include <stm32f4xx_spi.h>
#include <stm32f4xx_rcc.h>

namespace ecl
{

template< spi_device        dev,
          std::uintptr_t    dma_tx_stream,
          uint32_t          dma_tx_channel,
          std::uintptr_t    dma_rx_stream,
          uint32_t          dma_rx_channel,
          uint16_t          direction,
          uint16_t          mode,
          uint16_t          cpol,
          uint16_t          cpha,
          uint16_t          nss,
          uint16_t          first_bit,
          uint32_t          clk >
struct spi_config
{
    static constexpr SPI_InitTypeDef m_init_obj = {
        direction,
        mode,
        SPI_DataSize_8b,
        cpol,
        cpha,
        nss,
        SPI_BaudRatePrescaler_8, // Initial prescaler
        first_bit,
        7
    };

    static constexpr uint32_t           m_clk              = clk;
    static constexpr spi_device         m_dev              = dev;
    static constexpr uint32_t           m_dma_tx_channel   = dma_tx_channel;
    static constexpr std::uintptr_t     m_dma_tx_stream    = dma_tx_stream;
    static constexpr uint32_t           m_dma_rx_channel   = dma_rx_channel;
    static constexpr std::uintptr_t     m_dma_rx_stream    = dma_rx_stream;
};

template< class spi_config >
class spi_bus
{
public:
    // Convinient type aliases.
    using channel       = bus_channel;
    using event         = bus_event;
    using handler_fn    = bus_handler;

    //!
    //! \brief Constructs a bus.
    //!
    spi_bus();

    //!
    //! \brief Destructs a bus.
    ~spi_bus();

    //!
    //! \brief Lazy initialization.
    //! \return Status of opeartion.
    //!
    ecl::err init();

    //!
    //! \brief Sets rx buffer with given size.
    //! \param[in,out]  rx      Buffer to write data to. Optional.
    //! \param[in]      size    Size
    //!
    void set_rx(uint8_t *rx, size_t size);

    //!
    //! \brief Sets rx buffer made-up from sequence of similar bytes.
    //! \param[in] size         Size of sequence
    //! \param[in] fill_byte    Byte to fill a sequence. Optional.
    //!
    void set_tx(size_t size, uint8_t fill_byte = 0xff);

    //!
    //! \brief Sets tx buffer with given size.
    //! \param[in] tx   Buffer to transmit. Optional.
    //! \param[in] size Buffer size.
    //!
    void set_tx(const uint8_t *tx, size_t size);

    //!
    //! \brief Sets event handler.
    //! Handler will be used by the bus, until reset_handler() will be called.
    //! \param[in] handler Handler itself.
    //!
    void set_handler(const handler_fn &handler);

    //!
    //! \brief Reset xfer buffers.
    //! Buffers that were set by \sa set_tx() and \sa set_rx()
    //! will be no longer used after this call.
    //!
    void reset_buffers();

    //!
    //! \brief Resets previously set handler.
    //!
    void reset_handler();

    //!
    //! \brief Executes xfer, using buffers previously set.
    //! When it will be done, handler will be invoked.
    //! \return Status of operation.
    //!
    ecl::err do_xfer();

private:
    static constexpr auto pick_spi();
    static constexpr auto pick_rcc();
    static constexpr auto pick_rcc_fn();
//    static constexpr auto pick_IT();

    // Depends on the evironment
    static auto pick_pclk();

    // DMA init helper
    void init_dma();

    // IRQ init helper. TODO: decide if this needed or not
    // void init_irq();

    // Handles both DMA and SPI IRQ events. (for now it handles DMA only)
    void irq_handler();

    //! Bus is inited if this flag is set.
    static constexpr uint8_t inited         = 0x1;
    //! Bus is in fill mode if this flag is set.
    static constexpr uint8_t mode_fill      = 0x2;

    handler_fn      m_event_handler; //! Handler passed via set_handler().
    union
    {
        const uint8_t   *buf;        //! Transmit buffer.
        uint8_t          byte;       //! Byte to transmit.
    } m_tx;

    size_t          m_tx_size;       //! TX buffer size.
    uint8_t         *m_rx;           //! Recieve buffer.
    size_t          m_rx_size;       //! RX buffer size.
    uint8_t         m_status;        //! Represents bus status.
};

template< class spi_config >
spi_bus< spi_config >::spi_bus()
    :m_event_handler{false}
    ,m_tx{nullptr}
    ,m_tx_size{0}
    ,m_rx{nullptr}
    ,m_rx_size{0}
    ,m_status{0}
{

}

template< class spi_config >
spi_bus< spi_config >::~spi_bus()
{

}

template< class spi_config >
ecl::err spi_bus< spi_config >::init()
{
    if (m_status & inited) {
        return ecl::err::ok;
    }

    constexpr auto spi               = pick_spi();
    constexpr auto rcc_periph        = pick_rcc();
    constexpr auto rcc_fn            = pick_rcc_fn();

    // Why it does work in so strange way?
    constexpr auto init_const_obj = spi_config::m_init_obj;
    auto init_obj = init_const_obj;

    // Current clock on corresponding bus
    auto apb_clk = pick_pclk();

    // To calculate closest supported clock:
    uint16_t quotient = apb_clk / spi_config::m_clk;

    // Prescaler has a range from 2 to 256
    if (quotient < 2) {
        quotient = 2;
    } else if (quotient > 256) {
        quotient = 256;
    }

    // Divider values maps to prescaler according to
    // binary logarithm in following way:
    // Quo   Log2   Prescaler
    // 2     1      0   (0b00)
    // 4     2      1   (0b01)
    // 8     3      2   (0b10)
    // 16    4      3   (0b11)
    // So conversion formula will look like:
    // prescaler = log2(divider) - 1;
    // Using clz() is more efficient way to do
    // it and covers case when quotient is not
    // power of two value.
    uint16_t presc = ((32 - 1 - __builtin_clz(quotient)) - 1) << 3;
    init_obj.SPI_BaudRatePrescaler = presc;

    rcc_fn(rcc_periph, ENABLE);
    SPI_Init(spi, &init_obj);

    // TODO: check order
    init_dma();

    m_status |= inited;

    return ecl::err::ok;
}

template< class spi_config >
void spi_bus< spi_config >::set_rx(uint8_t *rx, size_t size)
{
    m_rx = rx;
    m_rx_size = size;
}

template< class spi_config >
void spi_bus< spi_config >::set_tx(size_t size, uint8_t fill_byte)
{
    m_status    |= mode_fill;
    m_tx.byte   = fill_byte;
    m_tx_size   = size;
}


template< class spi_config >
void spi_bus< spi_config >::set_tx(const uint8_t *tx, size_t size)
{
    m_status    &= ~(mode_fill);
    m_tx.buf    = tx;
    m_tx_size   = size;
}


template< class spi_config >
void spi_bus< spi_config >::set_handler(const handler_fn &handler)
{
    m_event_handler = handler;
}


template< class spi_config >
void spi_bus< spi_config >::reset_buffers()
{
    m_status    &= ~(mode_fill);
    m_tx.buf    = nullptr;
    m_tx_size   = 0;
    m_rx        = nullptr;
    m_rx_size   = 0;
}

template< class spi_config >
void spi_bus< spi_config >::reset_handler()
{
    m_event_handler = handler_fn{};
}

template< class spi_config >
ecl::err spi_bus< spi_config >::do_xfer()
{
    return ecl::err::ok;
}


//------------------------------------------------------------------------------

template< class spi_config >
constexpr auto spi_bus< spi_config >::pick_spi()
{
    switch (spi_config::m_dev) {
    case spi_device::bus_1:
        return SPI1;
    case spi_device::bus_2:
        return SPI2;
    case spi_device::bus_3:
        return SPI3;
    case spi_device::bus_4:
        return SPI4;
    case spi_device::bus_5:
        return SPI5;
    case spi_device::bus_6:
        return SPI6;
    default:
        // TODO: clarfy
        return static_cast< decltype(SPI1) >(nullptr);
    }
}

template< class spi_config >
constexpr auto spi_bus< spi_config >::pick_rcc()
{
    // TODO: comments
    switch (spi_config::m_dev) {
    case spi_device::bus_1:
        return RCC_APB2Periph_SPI1;
    case spi_device::bus_2:
        return RCC_APB1Periph_SPI2;
    case spi_device::bus_3:
        return RCC_APB1Periph_SPI3;
    case spi_device::bus_4:
        return RCC_APB2Periph_SPI4;
    case spi_device::bus_5:
        return RCC_APB2Periph_SPI5;
    case spi_device::bus_6:
        return RCC_APB2Periph_SPI6;
    default:
        // TODO: clarify
        return static_cast< decltype(RCC_APB2Periph_SPI6) >(-1);
    }
}

template< class spi_config >
constexpr auto spi_bus< spi_config >::pick_rcc_fn()
{
    // APB1 - SPI3 SPI2
    // APB2 - SPI5 SPI6 SPI1 SPI4
    switch (spi_config::m_dev) {
    case spi_device::bus_1:
    case spi_device::bus_5:
    case spi_device::bus_4:
    case spi_device::bus_6:
        return RCC_APB2PeriphClockCmd;
    case spi_device::bus_2:
    case spi_device::bus_3:
        return RCC_APB1PeriphClockCmd;
    default:
        // TODO: clarify
        return static_cast< decltype(&RCC_APB2PeriphClockCmd) >(nullptr);
    }
}

template< class spi_config >
auto spi_bus< spi_config >::pick_pclk()
{
    RCC_ClocksTypeDef clkcfg;
    RCC_GetClocksFreq(&clkcfg);

    switch (spi_config::m_dev) {
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

template< class spi_config >
void spi_bus< spi_config >::init_dma()
{
    dma::init_rcc< spi_config::m_dma_rx_stream >();
    dma::init_rcc< spi_config::m_dma_tx_stream >();

    auto handler = [this]() {
        this->irq_handler();
    };

    dma::subscribe_irq< spi_config::m_dma_rx_stream >(handler);
    dma::subscribe_irq< spi_config::m_dma_rx_stream >(handler);
}

template< class spi_config >
void spi_bus< spi_config >::irq_handler()
{

}

} // namespace ecl

#endif // PLATFORM_SPI_BUS_HPP_
