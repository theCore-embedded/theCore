//!
//! \file
//! \brief Contains platform driver implementation for SPI and I2S buses.
//! \details In STM32F4 series SPI and I2S are implemented within the same controller.
//!
#ifndef PLATFORM_SPI_BUS_HPP_
#define PLATFORM_SPI_BUS_HPP_

#include "aux/dma_device.hpp"

#include <common/spi.hpp>
#include <platform/common/bus.hpp>
#include <platform/irq_manager.hpp>

#include <sys/types.h>

#include <stm32f4xx_spi.h>
#include <stm32f4xx_rcc.h>

#include <type_traits>

namespace ecl
{

//! \brief Defines type of the bus
//!
enum class spi_bus_type
{
    spi,
    i2s
};

//! \brief Defines configuration for I2S bus
//!  Provides interface_init() function to init I2S interface.
//!
template <uint16_t  mode,
          uint16_t  standard,
          uint16_t  data_format,
          uint16_t  mclk_output,
          uint16_t  audio_freq,
          uint16_t  cpol >
struct i2s_config
{
    static constexpr I2S_InitTypeDef m_init_obj = {
            .I2S_Mode = mode,
            .I2S_Standard = standard,
            .I2S_DataFormat = data_format,
            .I2S_MCLKOutput = mclk_output,
            .I2S_AudioFreq = audio_freq,
            .I2S_CPOL = cpol
    };

    static void interface_init(SPI_TypeDef *i2s)
    {
        RCC_PLLI2SCmd(ENABLE);

        constexpr auto init_const_obj = m_init_obj;
        auto init_obj = init_const_obj;
        I2S_Init(i2s, &init_obj);

        // TODO: disable I2S when there in no XFER
        I2S_Cmd(i2s, ENABLE);
    }

    static constexpr auto m_bus_type = spi_bus_type::i2s;
};

//! \brief Defines configuration for SPI bus.
//!  Provides interface_init() function to init SPI interface.
//!
template< uint16_t  direction,
          uint16_t  mode,
          uint16_t  cpol,
          uint16_t  cpha,
          uint16_t  nss,
          uint16_t  first_bit,
          uint32_t  baud_rate_prescaler >
struct spi_config
{
    static constexpr SPI_InitTypeDef m_init_obj = {
        .SPI_Direction = direction,
        .SPI_Mode = mode,
        .SPI_DataSize = SPI_DataSize_8b,
        .SPI_CPOL = cpol,
        .SPI_CPHA = cpha,
        .SPI_NSS = nss,
        .SPI_BaudRatePrescaler = baud_rate_prescaler,
        .SPI_FirstBit = first_bit,
        .SPI_CRCPolynomial = 7
    };

    static void interface_init(SPI_TypeDef *spi)
    {
        constexpr auto init_const_obj = m_init_obj;
        auto init_obj = init_const_obj;
        SPI_Init(spi, &init_obj);

        // TODO: disable SPI when there in no XFER
        SPI_Cmd(spi, ENABLE);
    }

    static constexpr auto m_bus_type = spi_bus_type::spi;
};

//! \brief Defines common configuration for driver.
//!  Provides init function which is used to initialize low-level interface.
//!
template < spi_device        dev,
           std::uintptr_t    dma_tx_stream,
           uint32_t          dma_tx_channel,
           std::uintptr_t    dma_rx_stream,
           uint32_t          dma_rx_channel,
           class             bus_config >
struct spi_i2s_config
{
    static constexpr spi_device         m_dev              = dev;
    static constexpr uint32_t           m_dma_tx_channel   = dma_tx_channel;
    static constexpr std::uintptr_t     m_dma_tx_stream    = dma_tx_stream;
    static constexpr uint32_t           m_dma_rx_channel   = dma_rx_channel;
    static constexpr std::uintptr_t     m_dma_rx_stream    = dma_rx_stream;
    static constexpr auto               m_bus_type         = bus_config::m_bus_type;
    static constexpr auto               m_bus_cfg          = bus_config::m_init_obj;

    static void init(SPI_TypeDef *spi)
    {
        bus_config::interface_init(spi);
    }
};

//! \brief Driver implementation for SPI and I2S
//! \tparam spi_i2s_config common configuration
//!
template< class spi_i2s_config >
class spi_i2s_bus
{
public:
    // Convenient type aliases.
    using channel       = bus_channel;
    using event         = bus_event;
    using handler_fn    = bus_handler;

    //!
    //! \brief Constructs a bus.
    //!
    spi_i2s_bus();

    //!
    //! \brief Destructs a bus.
    ~spi_i2s_bus();

    //!
    //! \brief Lazy initialization.
    //! \return Status of operation.
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

    //!
    //! \brief Sets audio frequency for I2S mode.
    //! \details Is not used in SPI mode.
    //! \tparam frequency Can be a type of ecl::i2s::audio_frequency
    //! \tparam Iface_cfg Used for correct enable_if mechanism. Must not be passed by user.
    //! \return Status of operation.
    //!
    template < ecl::i2s::audio_frequency frequency, class Iface_cfg = spi_i2s_config >
    std::enable_if_t< Iface_cfg::m_bus_type == spi_bus_type::i2s, ecl::err >
    i2s_set_audio_frequency();

    //!
    //! \brief Sets data format for I2S mode.
    //! \details Is not used in SPI mode.
    //! \tparam format Can be a type of ecl::i2s::data_format
    //! \tparam Iface_cfg Used for correct enable_if mechanism. Must not be passed by user.
    //! \return Status of operation.
    //!
    template < ecl::i2s::data_format format, class Iface_cfg = spi_i2s_config >
    std::enable_if_t< Iface_cfg::m_bus_type == spi_bus_type::i2s, ecl::err >
    i2s_set_data_format();

private:
    static constexpr auto pick_spi();
    static constexpr auto pick_rcc();
    static constexpr auto pick_rcc_fn();
    static constexpr uint32_t pick_i2s_audio_frequency(ecl::i2s::audio_frequency value);
    static constexpr uint32_t pick_i2s_data_format(ecl::i2s::data_format value);

    ecl::err i2s_set_audio_frequency_private(uint32_t value);
    ecl::err i2s_set_data_format_private(uint32_t value);

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
        uint16_t         byte;       //! Byte to transmit (16 bits of 16-bit SPI mode)
    } m_tx;

    size_t          m_tx_size;       //! TX buffer size.
    uint8_t         *m_rx;           //! Receive buffer.
    size_t          m_rx_size;       //! RX buffer size.
    uint8_t         m_status;        //! Represents bus status.
};

template< class spi_i2s_config >
spi_i2s_bus< spi_i2s_config >::spi_i2s_bus()
    :m_event_handler{false}
    ,m_tx{nullptr}
    ,m_tx_size{0}
    ,m_rx{nullptr}
    ,m_rx_size{0}
    ,m_status{0}
{

}

template< class spi_i2s_config >
spi_i2s_bus< spi_i2s_config >::~spi_i2s_bus()
{

}

template< class spi_i2s_config >
ecl::err spi_i2s_bus< spi_i2s_config >::init()
{
    if (m_status & inited) {
        return ecl::err::ok;
    }

    constexpr auto spi               = pick_spi();
    constexpr auto rcc_periph        = pick_rcc();
    constexpr auto rcc_fn            = pick_rcc_fn();

    rcc_fn(rcc_periph, ENABLE);

    init_dma();

    spi_i2s_config::init(spi);

    m_status |= inited;

    return ecl::err::ok;
}

template< class spi_i2s_config >
void spi_i2s_bus< spi_i2s_config >::set_rx(uint8_t *rx, size_t size)
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

template< class spi_i2s_config >
void spi_i2s_bus< spi_i2s_config >::set_tx(size_t size, uint8_t fill_byte)
{
    if (!(m_status & inited)) {
        return;
    }

    m_status    |= mode_fill;
    m_status    &= ~(tx_hidden);
    m_tx.byte   = fill_byte;
    m_tx_size   = size;
}

template< class spi_i2s_config >
void spi_i2s_bus< spi_i2s_config >::set_tx(const uint8_t *tx, size_t size)
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


template< class spi_i2s_config >
void spi_i2s_bus< spi_i2s_config >::set_handler(const handler_fn &handler)
{
    m_event_handler = handler;
}


template< class spi_i2s_config >
void spi_i2s_bus< spi_i2s_config >::reset_buffers()
{
    m_status    &= ~(mode_fill);
    m_tx.buf    = nullptr;
    m_tx_size   = 0;
    m_rx        = nullptr;
    m_rx_size   = 0;
}

template< class spi_i2s_config >
void spi_i2s_bus< spi_i2s_config >::reset_handler()
{
    m_event_handler = handler_fn{};
}

template< class spi_i2s_config >
ecl::err spi_i2s_bus< spi_i2s_config >::do_xfer()
{
    if (!(m_status & inited)) {
        return err::perm;
    }

    if (!valid_sizes()) {
        return err::nobufs;
    }

    // TODO: check if buffers are the same as in previous transaction
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

template< class spi_i2s_config >
bool spi_i2s_bus< spi_i2s_config >::valid_sizes()
{
    // Bus is in full-duplex mode. Different sizes are not permitted.
    if (m_tx_size && m_rx_size) {
        if (m_tx_size != m_rx_size) {
            return false;
        }
    }

    return true;
}

template< class spi_i2s_config >
void spi_i2s_bus< spi_i2s_config >::prepare_tx()
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

    constexpr auto tx_dma   = dma::get_stream< spi_i2s_config::m_dma_tx_stream >();
    constexpr auto spi      = pick_spi();

    DMA_InitTypeDef dma_init;
    DMA_StructInit(&dma_init);

    dma_init.DMA_Channel             = spi_i2s_config::m_dma_tx_channel;
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

    if (spi_i2s_config::m_bus_type == spi_bus_type::i2s) {
        dma_init.DMA_MemoryDataSize      = DMA_MemoryDataSize_HalfWord;
        dma_init.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_HalfWord;
        dma_init.DMA_BufferSize          = m_tx_size / 2;
    } else {
        dma_init.DMA_BufferSize          = m_tx_size;
    }

    DMA_Init(tx_dma, &dma_init);
    DMA_ITConfig(tx_dma, DMA_IT_TC | DMA_IT_HT, ENABLE);
}

template< class spi_i2s_config >
void spi_i2s_bus< spi_i2s_config >::prepare_rx()
{
    if (!m_rx_size) {
        // Preventing event routine from waiting RX complete event.
        m_status |= rx_complete;
        return;
    }

    m_status &= ~(rx_complete);

    constexpr auto rx_dma   = dma::get_stream< spi_i2s_config::m_dma_rx_stream >();
    constexpr auto spi      = pick_spi();

    DMA_InitTypeDef dma_init;
    DMA_StructInit(&dma_init);

    dma_init.DMA_Channel             = spi_i2s_config::m_dma_rx_channel;
    dma_init.DMA_DIR                 = DMA_DIR_PeripheralToMemory;
    dma_init.DMA_PeripheralBaseAddr  = reinterpret_cast< uint32_t >(&spi->DR);
    dma_init.DMA_MemoryInc           = DMA_MemoryInc_Enable;
    dma_init.DMA_Memory0BaseAddr     = reinterpret_cast< uint32_t >(m_rx);

    if (spi_i2s_config::m_bus_type == spi_bus_type::i2s) {
        dma_init.DMA_MemoryDataSize      = DMA_MemoryDataSize_HalfWord;
        dma_init.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_HalfWord;
        dma_init.DMA_BufferSize          = m_rx_size / 2;
    } else {
        dma_init.DMA_BufferSize          = m_rx_size;
    }

    DMA_Init(rx_dma, &dma_init);
    DMA_ITConfig(rx_dma, DMA_IT_TC | DMA_IT_HT, ENABLE);
}

template< class spi_i2s_config >
void spi_i2s_bus< spi_i2s_config >::start_xfer()
{
    constexpr auto rx_dma = dma::get_stream< spi_i2s_config::m_dma_rx_stream >();
    constexpr auto tx_dma = dma::get_stream< spi_i2s_config::m_dma_tx_stream >();
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

template< class spi_i2s_config >
void spi_i2s_bus< spi_i2s_config >::irq_handler()
{
    constexpr auto rx_irqn = dma::get_irqn< spi_i2s_config::m_dma_rx_stream >();
    constexpr auto tx_irqn = dma::get_irqn< spi_i2s_config::m_dma_tx_stream >();

    constexpr auto tx_dma   = dma::get_stream< spi_i2s_config::m_dma_tx_stream >();
    constexpr auto rx_dma   = dma::get_stream< spi_i2s_config::m_dma_rx_stream >();

    constexpr auto tx_tc_if = dma::get_tc_if< spi_i2s_config::m_dma_tx_stream >();
    constexpr auto rx_tc_if = dma::get_tc_if< spi_i2s_config::m_dma_rx_stream >();

    constexpr auto tx_ht_if = dma::get_ht_if< spi_i2s_config::m_dma_tx_stream >();
    constexpr auto rx_ht_if = dma::get_ht_if< spi_i2s_config::m_dma_rx_stream >();

    constexpr auto spi = pick_spi();

    if (!(m_status & tx_complete)) {
        if (DMA_GetITStatus(tx_dma, tx_tc_if)) {
            // Complete TX transaction
            constexpr auto tx_tc_flag = dma::get_tc_flag< spi_i2s_config::m_dma_tx_stream >();

            if (!(m_status & tx_hidden)) {
                m_event_handler(channel::tx, event::tc, m_tx_size);
            }

            DMA_ClearFlag(tx_dma, tx_tc_flag);
            DMA_ClearITPendingBit(tx_dma, tx_tc_if);
            DMA_ITConfig(tx_dma, DMA_IT_TC | DMA_IT_HT, DISABLE);

            m_status |= tx_complete;
        } else if (DMA_GetITStatus(tx_dma, tx_ht_if)) {
            constexpr auto tx_ht_flag = dma::get_ht_flag< spi_i2s_config::m_dma_tx_stream >();

            uint32_t tx_left = DMA_GetCurrDataCounter(tx_dma);

            if (spi_i2s_config::m_bus_type == spi_bus_type::i2s) {
                tx_left *= 2;
            }

            if (!(m_status & tx_hidden)) {
                m_event_handler(channel::tx, event::ht, m_tx_size - tx_left);
            }

            DMA_ClearFlag(tx_dma, tx_ht_flag);
            DMA_ClearITPendingBit(tx_dma, tx_ht_if);

            irq_manager::clear(tx_irqn);
            irq_manager::unmask(tx_irqn);
        }
    }

    if (!(m_status & rx_complete)) {
        if (DMA_GetITStatus(rx_dma, rx_tc_if)) {
            // Complete TX transaction
            constexpr auto rx_tc_flag = dma::get_tc_flag< spi_i2s_config::m_dma_rx_stream >();

            m_event_handler(channel::rx, event::tc, m_rx_size);

            DMA_ClearFlag(rx_dma, rx_tc_flag);
            DMA_ClearITPendingBit(rx_dma, rx_tc_if);
            DMA_ITConfig(rx_dma, DMA_IT_TC | DMA_IT_HT, DISABLE);

            m_status |= rx_complete;
        } else if (DMA_GetITStatus(rx_dma, rx_ht_if)) {
            constexpr auto rx_ht_flag = dma::get_ht_flag< spi_i2s_config::m_dma_tx_stream >();

            uint32_t rx_left = DMA_GetCurrDataCounter(rx_dma);

            if (spi_i2s_config::m_bus_type == spi_bus_type::i2s) {
                rx_left *= 2;
            }

            m_event_handler(channel::rx, event::ht, m_rx_size - rx_left);

            DMA_ClearFlag(rx_dma, rx_ht_flag);
            DMA_ClearITPendingBit(rx_dma, rx_ht_if);

            irq_manager::clear(rx_irqn);
            irq_manager::unmask(rx_irqn);
        }
    }

    if ((m_status & (rx_complete | tx_complete)) == (rx_complete | tx_complete)) {

        DMA_Cmd(tx_dma, DISABLE);
        DMA_DeInit(tx_dma);

        DMA_Cmd(rx_dma, DISABLE);
        DMA_DeInit(rx_dma);

        SPI_I2S_DMACmd(spi, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);

        // Clear/enable NVIC interrupts
        irq_manager::clear(rx_irqn);
        irq_manager::unmask(rx_irqn);
        irq_manager::clear(tx_irqn);
        irq_manager::unmask(tx_irqn);

        // All transfers completed
        m_event_handler(channel::meta, event::tc, m_tx_size);
    }
}

template< class spi_i2s_config >
constexpr auto spi_i2s_bus< spi_i2s_config >::pick_spi()
{
    switch (spi_i2s_config::m_dev) {
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
        // TODO: clarify
        return static_cast< decltype(SPI1) >(nullptr);
    }
}

template< class spi_i2s_config >
constexpr auto spi_i2s_bus< spi_i2s_config >::pick_rcc()
{
    // TODO: comments
    switch (spi_i2s_config::m_dev) {
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

template< class spi_i2s_config >
constexpr auto spi_i2s_bus< spi_i2s_config >::pick_rcc_fn()
{
    // APB1 - SPI3 SPI2
    // APB2 - SPI5 SPI6 SPI1 SPI4
    switch (spi_i2s_config::m_dev) {
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

template< class spi_i2s_config >
constexpr uint32_t spi_i2s_bus< spi_i2s_config >::pick_i2s_audio_frequency(ecl::i2s::audio_frequency value)
{
    switch (value) {
    case ecl::i2s::audio_frequency::k8:
        return I2S_AudioFreq_8k;
    case ecl::i2s::audio_frequency::k11:
        return I2S_AudioFreq_11k;
    case ecl::i2s::audio_frequency::k16:
        return I2S_AudioFreq_16k;
    case ecl::i2s::audio_frequency::k22:
        return I2S_AudioFreq_22k;
    case ecl::i2s::audio_frequency::k32:
        return I2S_AudioFreq_32k;
    case ecl::i2s::audio_frequency::k44:
        return I2S_AudioFreq_44k;
    case ecl::i2s::audio_frequency::k48:
        return I2S_AudioFreq_48k;
    case ecl::i2s::audio_frequency::k96:
        return I2S_AudioFreq_96k;
    case ecl::i2s::audio_frequency::k192:
        return I2S_AudioFreq_192k;
    default:
        return static_cast< decltype(I2S_AudioFreq_192k) >(0);
    }
}

template< class spi_i2s_config >
constexpr uint32_t spi_i2s_bus< spi_i2s_config >::pick_i2s_data_format(ecl::i2s::data_format value)
{
    switch (value) {
    case ecl::i2s::data_format::b16:
        return I2S_DataFormat_16b;
    case ecl::i2s::data_format::b24:
        return I2S_DataFormat_24b;
    case ecl::i2s::data_format::b32:
        return I2S_DataFormat_32b;
    default:
        return static_cast< decltype(I2S_DataFormat_32b) >(0);
    }
}

template< class spi_i2s_config >
void spi_i2s_bus< spi_i2s_config >::init_dma()
{
    dma::init_rcc< spi_i2s_config::m_dma_rx_stream >();
    dma::init_rcc< spi_i2s_config::m_dma_tx_stream >();

    auto handler = [this]() {
        this->irq_handler();
    };

    dma::subscribe_irq< spi_i2s_config::m_dma_rx_stream >(handler);
    dma::subscribe_irq< spi_i2s_config::m_dma_tx_stream >(handler);
}

template< class spi_i2s_config >
template< ecl::i2s::audio_frequency frequency, class Iface_cfg >
std::enable_if_t< Iface_cfg::m_bus_type == spi_bus_type::i2s, ecl::err >
spi_i2s_bus< spi_i2s_config >::i2s_set_audio_frequency()
{
    static_assert(frequency != ecl::i2s::audio_frequency::k88, "frequency is not supported by a platform");

    return i2s_set_audio_frequency_private(pick_i2s_audio_frequency(frequency));
}

template< class spi_i2s_config >
ecl::err spi_i2s_bus< spi_i2s_config >::i2s_set_audio_frequency_private(uint32_t value)
{
    constexpr auto spi = pick_spi();
    // TODO check if spi is inited

    // Disable I2S before changing any parameters according to RM
    I2S_Cmd(spi, DISABLE);

    // There is a complex procedure of setting correct
    // prescaler to achieve necessary frequency.
    // It is done in I2S_Init(), so we can reuse it.

    // can be a little confusing, we cannot use i2s_init = spi_i2s_config::m_bus_cfg,
    // since spi_i2s_config::m_bus_cfg is calculated in compile time, so the structure
    // itself is not created. Linker is very upset about it.
    I2S_InitTypeDef i2s_init;
    i2s_init.I2S_Standard = spi_i2s_config::m_bus_cfg.I2S_DataFormat;
    i2s_init.I2S_CPOL = spi_i2s_config::m_bus_cfg.I2S_CPOL;
    i2s_init.I2S_MCLKOutput = spi_i2s_config::m_bus_cfg.I2S_MCLKOutput;
    i2s_init.I2S_Mode = spi_i2s_config::m_bus_cfg.I2S_Mode;

    // Data format can be changed by i2s_set_data_format()
    i2s_init.I2S_DataFormat = spi->I2SCFGR & SPI_I2SCFGR_DATLEN;
    // change audio frequency
    i2s_init.I2S_AudioFreq = value;

    // re-init I2S with new audio frequency
    I2S_Init(spi, &i2s_init);

    I2S_Cmd(spi, ENABLE);

    return ecl::err::ok;
}

template< class spi_i2s_config >
template< ecl::i2s::data_format format, class Iface_cfg >
std::enable_if_t< Iface_cfg::m_bus_type == spi_bus_type::i2s, ecl::err >
spi_i2s_bus< spi_i2s_config >::i2s_set_data_format()
{
    static_assert(format != ecl::i2s::data_format::b8, "format is not supported by a platform");

    return i2s_set_data_format_private(pick_i2s_data_format(format));
}

template< class spi_i2s_config >
ecl::err spi_i2s_bus< spi_i2s_config >::i2s_set_data_format_private(uint32_t value)
{
    constexpr auto spi = pick_spi();
    // TODO check if spi is inited

    // Disable I2S before changing any parameters according to RM
    I2S_Cmd(spi, DISABLE);

    spi->I2SCFGR |= value;

    I2S_Cmd(spi, ENABLE);

    return ecl::err::ok;
}

} // namespace ecl

#endif // PLATFORM_SPI_BUS_HPP_
