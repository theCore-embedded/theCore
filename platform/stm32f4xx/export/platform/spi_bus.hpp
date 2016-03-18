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

    // Checks if buffer sizes are valid
    bool valid_sizes();

    // Prepares TX transaction, if needed
    void prepare_tx();

    // Prepares RX transaction, if needed
    void prepare_rx();

    // Starts transaction, if needed
    void start_xfer();

    // IRQ init helper. TODO: decide if this needed or not
    // void init_irq();

    // Handles both DMA and SPI IRQ events. (for now it handles DMA only)
    void irq_handler();

    //! Bus is inited if this flag is set.
    static constexpr uint8_t inited         = 0x1;
    //! Bus is in fill mode if this flag is set.
    static constexpr uint8_t mode_fill      = 0x2;
    //! TX is finished if this flag is set.
    static constexpr uint8_t tx_complete    = 0x4;
    //! User will not be notified about TX events if this flag is set.
    static constexpr uint8_t tx_hidden      = 0x8;
    //! RX is finished if this flag is set.
    static constexpr uint8_t rx_complete    = 0x10;

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

    // TODO: check order (do it need to be swapped with SPI_init()? )
    init_dma();

    // TODO: disable SPI when there in no XFER
    SPI_Cmd(spi, ENABLE);

    m_status |= inited;

    return ecl::err::ok;
}

template< class spi_config >
void spi_bus< spi_config >::set_rx(uint8_t *rx, size_t size)
{
    if (!(m_status & inited)) {
        return;
    }

    // Threat nullptr buffers as indication that xfer of this type
    // is not required
    if (!rx) {
        return;
    }

    m_rx = rx;
    m_rx_size = size;
}

template< class spi_config >
void spi_bus< spi_config >::set_tx(size_t size, uint8_t fill_byte)
{
    if (!(m_status & inited)) {
        return;
    }

    m_status    |= mode_fill;
    m_status    &= ~(tx_hidden);
    m_tx.byte   = fill_byte;
    m_tx_size   = size;
}

template< class spi_config >
void spi_bus< spi_config >::set_tx(const uint8_t *tx, size_t size)
{
    if (!(m_status & inited)) {
        return;
    }

    // Threat nullptr buffers as indication that xfer of this type
    // is not required
    if (!tx) {
        // TX channel must be enabled in bidir SPI even if user don't
        // want it.
        m_status |= tx_hidden;
        return;
    }

    m_status    &= ~(mode_fill);
    m_status    &= ~(tx_hidden);
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
    if (!(m_status & inited)) {
        return err::perm;
    }

    if (!valid_sizes()) {
        return err::nobufs;
    }

    // TODO: check if buffers are the same as in previous transacuib
    // If so, do not reinitialize DMA but rather just update a data counter.


    // TODO: wait for BSY flag before we go
    //    while (SPI_I2S_GetFlagStatus(spi, SPI_FLAG_TXE) == RESET)
    //    {}
    //    while (SPI_I2S_GetFlagStatus(spi, SPI_FLAG_BSY) == SET)
    //    {}

    prepare_tx();
    prepare_rx();
    start_xfer();

    return ecl::err::ok;
}

//------------------------------------------------------------------------------

template< class spi_config >
bool spi_bus< spi_config >::valid_sizes()
{
    // Bus is in full-duplex mode. Different sizes are not permitted.
    if (m_tx_size && m_rx_size) {
        if (m_tx_size != m_rx_size) {
            return false;
        }
    }

    return true;
}

template< class spi_config >
void spi_bus< spi_config >::prepare_tx()
{
    if (m_status & tx_hidden) {
        // TX is not requested by user but it is required to setup this stream
        // in SPI in order to receive data from RX channel.
        // Trick is to use fill mode in TX channel without notifying
        // user about it.

        if (!m_rx_size) {
            // TODO: is this correct that both buffers with 0 size???
            for(;;);
        }

        m_status    |= mode_fill;
        m_tx_size   = m_rx_size;
        m_tx.byte   = 0xff;
    }

    m_status &= ~(tx_complete);

    constexpr auto tx_dma   = dma::get_stream< spi_config::m_dma_tx_stream >();
    constexpr auto spi      = pick_spi();

    DMA_InitTypeDef dma_init;
    DMA_StructInit(&dma_init);

    dma_init.DMA_Channel             = spi_config::m_dma_tx_channel;
    dma_init.DMA_DIR                 = DMA_DIR_MemoryToPeripheral;
    dma_init.DMA_PeripheralBaseAddr  = reinterpret_cast< uint32_t >(&spi->DR);
    dma_init.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;

    if (m_status & mode_fill) {
        dma_init.DMA_MemoryInc       = DMA_MemoryInc_Disable;
        dma_init.DMA_Memory0BaseAddr = reinterpret_cast< uint32_t >(&m_tx.byte);
    } else {
        dma_init.DMA_MemoryInc       = DMA_MemoryInc_Enable;
        dma_init.DMA_Memory0BaseAddr = reinterpret_cast< uint32_t >(m_tx.buf);
    }

    dma_init.DMA_BufferSize          = m_tx_size;

    DMA_ITConfig(tx_dma, DMA_IT_TC, ENABLE);
    DMA_Init(tx_dma, &dma_init);
}

template< class spi_config >
void spi_bus< spi_config >::prepare_rx()
{
    if (!m_rx_size) {
        // Preventing event routine from waiting RX complete event.
        m_status |= rx_complete;
        return;
    }

    m_status &= ~(rx_complete);

    constexpr auto rx_dma   = dma::get_stream< spi_config::m_dma_rx_stream >();
    constexpr auto spi      = pick_spi();

    DMA_InitTypeDef dma_init;
    DMA_StructInit(&dma_init);

    dma_init.DMA_Channel             = spi_config::m_dma_rx_channel;
    dma_init.DMA_DIR                 = DMA_DIR_PeripheralToMemory;
    dma_init.DMA_PeripheralBaseAddr  = reinterpret_cast< uint32_t >(&spi->DR);
    dma_init.DMA_MemoryInc           = DMA_MemoryInc_Enable;
    dma_init.DMA_Memory0BaseAddr     = reinterpret_cast< uint32_t >(m_rx);
    dma_init.DMA_BufferSize          = m_rx_size;

    DMA_ITConfig(rx_dma, DMA_IT_TC, ENABLE);
    DMA_Init(rx_dma, &dma_init);
}

template< class spi_config >
void spi_bus< spi_config >::start_xfer()
{
    constexpr auto rx_dma = dma::get_stream< spi_config::m_dma_rx_stream >();
    constexpr auto tx_dma = dma::get_stream< spi_config::m_dma_tx_stream >();
    constexpr auto spi    = pick_spi();

    // After all directions configured, streams may be enabled
    if (m_tx_size) {
        DMA_Cmd(tx_dma, ENABLE);
    }

    if (m_rx_size) {
        DMA_Cmd(rx_dma, ENABLE);
    }

    // Enable interrupt request from SPI periphery
    SPI_I2S_DMACmd(spi, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx , ENABLE);
}

template< class spi_config >
void spi_bus< spi_config >::irq_handler()
{
    constexpr auto tx_dma   = dma::get_stream< spi_config::m_dma_tx_stream >();
    constexpr auto rx_dma   = dma::get_stream< spi_config::m_dma_rx_stream >();

    constexpr auto tx_tc_if = dma::get_tc_if< spi_config::m_dma_tx_stream >();
    constexpr auto rx_tc_if = dma::get_tc_if< spi_config::m_dma_rx_stream >();

    constexpr auto spi      = pick_spi();


    if (!(m_status & tx_complete)) {
        auto tx_tc = DMA_GetITStatus(tx_dma, tx_tc_if);

        if (tx_tc) {
            // Complete TX transaction
            constexpr auto tx_tc_flag = dma::get_tc_flag< spi_config::m_dma_tx_stream >();

            if (!(m_status & tx_hidden)) {
                m_event_handler(channel::tx, event::tc, m_tx_size);
            }

            DMA_ClearFlag(tx_dma, tx_tc_flag);
            DMA_ClearITPendingBit(tx_dma, tx_tc_if);
            DMA_ITConfig(tx_dma, DMA_IT_TC, DISABLE);

            m_status |= tx_complete;
        }
    }

    auto rx_tc = DMA_GetITStatus(rx_dma, rx_tc_if);

    if (!(m_status & rx_complete)) {
        if (rx_tc) {
            // Complete TX transaction
            constexpr auto rx_tc_flag = dma::get_tc_flag< spi_config::m_dma_rx_stream >();

            m_event_handler(channel::rx, event::tc, m_rx_size);

            DMA_ClearFlag(rx_dma, rx_tc_flag);
            DMA_ClearITPendingBit(rx_dma, rx_tc_if);
            DMA_ITConfig(rx_dma, DMA_IT_TC, DISABLE);

            m_status |= rx_complete;
        }
    }

    if ((m_status & (rx_complete | tx_complete)) == (rx_complete | tx_complete)) {
        constexpr auto rx_irqn = dma::get_irqn< spi_config::m_dma_rx_stream >();
        constexpr auto tx_irqn = dma::get_irqn< spi_config::m_dma_tx_stream >();

        DMA_Cmd(tx_dma, DISABLE);
        DMA_DeInit(tx_dma);

        DMA_Cmd(rx_dma, DISABLE);
        DMA_DeInit(rx_dma);

        SPI_I2S_DMACmd(spi, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);

        // Clear/enable NVIC interrupts
        IRQ_manager::clear(rx_irqn);
        IRQ_manager::unmask(rx_irqn);
        IRQ_manager::clear(tx_irqn);
        IRQ_manager::unmask(tx_irqn);

        // All transfers comepleted
        m_event_handler(channel::meta, event::tc, m_tx_size);
    }
}

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
    case spi_device::bus_1:
    case spi_device::bus_5:
    case spi_device::bus_4:
    case spi_device::bus_6:
        return clkcfg.PCLK2_Frequency;
    case spi_device::bus_2:
    case spi_device::bus_3:
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
    dma::subscribe_irq< spi_config::m_dma_tx_stream >(handler);
}


} // namespace ecl

#endif // PLATFORM_SPI_BUS_HPP_
