//!
//! \file
//! \brief Contains platform driver implementation for SPI and I2S buses.
//! \details In STM32F4 series SPI and I2S are implemented within the same controller.
//!
#ifndef PLATFORM_SPI_BUS_HPP_
#define PLATFORM_SPI_BUS_HPP_

#include "aux/dma_device.hpp"
#include "platform/irq_manager.hpp"

#include <common/spi.hpp>
#include <common/bus.hpp>

#include <stm32f4xx_spi.h>

#include <sys/types.h>

namespace ecl
{

//! \brief Defines type of the bus
//!
enum class spi_bus_type
{
    spi,
    i2s
};

//! Base template class for the SPI/I2S configuration
//! \details In order to advertise configuration parameters user must create
//! template specialization for required SPI/I2S device. Since STM32F4XX devices
//! support either the SPI protocol or the I2S audio protocol there are two sets
//! of parameters:
//!     - common parameters for I2S and SPI modes
//!     - special parameters for concrete mode.
//! An example is given below parameter specification. Refer to it to get
//! general idea how to configure SPI/I2S bus.
//!
//! Common parameters:
//! - ecl::spi_bus_type **bus_type** - Defines which bus mode is required,
//!   SPI or I2S. \sa ecl::spi_bus_type
//! - struct **dma** - Specifies DMA streams and channels. It must contain
//!   following static members:
//!     - ecl::dma::streams **tx_stream** - TX DMA stream number. Make sure right
//!       DMA stream is used for given SPI/I2S device. See "Table 42. DMA1 request
//!       mapping" and "Table 43. DMA2 request mapping" in the RM.
//!       \sa ecl::dma::streams
//!     - ecl::dma::streams **rx_stream** - RX DMA stream number. Make sure right
//!       DMA stream is used for given SPI/I2S device. See "Table 42. DMA1 request
//!       mapping" and "Table 43. DMA2 request mapping" in the RM.
//!       \sa ecl::dma::streams
//!     - uint32_t **tx_channel** - TX DMA channel. See "Table 42. DMA1 request
//!       mapping" and "Table 43. DMA2 request mapping" in the RM"
//!     - uint32_t **rx_channel** - RX RMA channel. See "Table 42. DMA1 request
//!       mapping" and "Table 43. DMA2 request mapping" in the RM.
//!
//! I2S-specific parameters:
//! - I2S_InitTypeDef **init_obj** - Configuraion structure required for I2S mode.
//!   Refer to STM32 SPL documentation or check examples below to find
//!   I2S_InitTypeDef's fields.
//!
//! SPI-specific parameters:
//! - SPI_InitTypeDef **init_obj** - Configuration structure required for SPI mode.
//!   Refer to STM32 SPL documentation or check examples below to find
//!   I2S_InitTypeDef's fields.
//!
//! \par SPI configuration example.
//! In order to use this configuration class one must create configuration class
//! in the `ecl` namespace before any acccess to \ref spi_i2s_bus instance.
//! \code{.cpp}
//! namespace ecl
//! {
//!
//! template<>
//! struct spi_i2s_cfg<spi_device::bus2>
//! {
//!     static constexpr auto bus_type = spi_bus_type::spi;
//!
//!     struct dma
//!     {
//!         static constexpr auto tx_stream  = ecl::dma::streams::dma1_4;
//!         static constexpr auto tx_channel = 0;
//!         static constexpr auto rx_stream  = ecl::dma::streams::dma1_3;
//!         static constexpr auto rx_channel = 0;
//!     };
//!
//!     static constexpr SPI_InitTypeDef init_obj = {
//!             .SPI_Direction          = SPI_Direction_2Lines_FullDuplex,
//!             .SPI_Mode               = SPI_Mode_Master,
//!             .SPI_DataSize           = SPI_DataSize_8b,
//!             .SPI_CPOL               = SPI_CPOL_High,
//!             .SPI_CPHA               = SPI_CPHA_2Edge,
//!             .SPI_NSS                = SPI_NSS_Soft,
//!             .SPI_BaudRatePrescaler  = SPI_BaudRatePrescaler_4,
//!             .SPI_FirstBit           = SPI_FirstBit_MSB,
//!             .SPI_CRCPolynomial      = 7,
//!     };
//! };
//!
//! } // namespace ecl
//! \endcode
//!
//! \par I2S configuration example.
//! In order to use this configuration class one must create configuration class
//! in the `ecl` namespace before any acccess to \ref spi_i2s_bus instance.
//! \code{.cpp}
//! namespace ecl
//! {
//!
//! template<>
//! struct spi_i2s_cfg<spi_device::bus2>
//! {
//!     static constexpr auto bus_type = spi_bus_type::i2s;
//!
//!     struct dma
//!     {
//!         static constexpr auto tx_stream  = ecl::dma::streams::dma1_4;
//!         static constexpr auto tx_channel = 0;
//!         static constexpr auto rx_stream  = ecl::dma::streams::dma1_3;
//!         static constexpr auto rx_channel = 0;
//!     };
//!
//!     static constexpr I2S_InitTypeDef init_obj = {
//!             .I2S_Mode          = I2S_Mode_MasterTx,
//!             .I2S_Standard      = I2S_Standard_Phillips,
//!             .I2S_DataFormat    = I2S_DataFormat_16b,
//!             .I2S_MCLKOutput    = I2S_MCLKOutput_Enable,
//!             .I2S_AudioFreq     = I2S_AudioFreq_44k,
//!             .I2S_CPOL          = I2S_CPOL_High,
//!     };
//! };
//!
//! } // namespace ecl
//! \endcode
//!
//! \warning To avoid potential problems with multiple configurations for single
//! SPI/I2S bus, **make sure that full specialization is placed in the
//! header included (directly or indirectly) by all dependent modules.**.
//! Thus, redefinition of the config class for given USART will result in
//! compilation errors. *Good practice is to place all USART configuration
//! class in the single target-related header.*
template<spi_device dev>
struct spi_i2s_cfg
{
    // Always assert
    static_assert(std::is_integral<decltype(dev)>::value,
                  "The instance of this generic class should never be "
                          "instantiated. Please write your own template specialization "
                          "of this class. See documentation.");
};


//! \brief Driver implementation for SPI and I2S
//! \tparam spi_i2s_config common configuration
//!
template<spi_device dev>
class spi_i2s_bus
{
public:
    // Convenient type aliases.
    using channel       = bus_channel;
    using event         = bus_event;
    using handler_fn    = bus_handler;
    using config        = spi_i2s_cfg<dev>;

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
    template<ecl::i2s::audio_frequency frequency, class Iface_cfg = config>
    std::enable_if_t<Iface_cfg::bus_type == spi_bus_type::i2s, ecl::err>
    i2s_set_audio_frequency();

    //!
    //! \brief Sets data format for I2S mode.
    //! \details Is not used in SPI mode.
    //! \tparam format Can be a type of ecl::i2s::data_format
    //! \tparam Iface_cfg Used for correct enable_if mechanism. Must not be passed by user.
    //! \return Status of operation.
    //!
    template<ecl::i2s::data_format format, class Iface_cfg = config>
    std::enable_if_t<Iface_cfg::bus_type == spi_bus_type::i2s, ecl::err>
    i2s_set_data_format();

private:
    static constexpr auto pick_spi();
    static constexpr auto pick_rcc();
    static constexpr auto pick_rcc_fn();

    static constexpr uint32_t pick_i2s_audio_frequency(ecl::i2s::audio_frequency value);
    static constexpr uint32_t pick_i2s_data_format(ecl::i2s::data_format value);
    ecl::err i2s_set_audio_frequency_private(uint32_t value);
    ecl::err i2s_set_data_format_private(uint32_t value);

    //! Initiliazes I2S interface, if bus was configured is such way
    template<class Iface_cfg = config>
    std::enable_if_t<Iface_cfg::bus_type == spi_bus_type::i2s, void>
    init_interface();

    //! Initiliazes SPI interface, if bus was configured is such way
    template<class Iface_cfg = config>
    std::enable_if_t<Iface_cfg::bus_type == spi_bus_type::spi, void>
    init_interface();

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

    handler_fn m_event_handler; //! Handler passed via set_handler().
    union
    {
        const uint8_t *buf;        //! Transmit buffer.
        uint16_t      byte;        //! Byte to transmit (16 bits of 16-bit SPI mode)
    } m_tx;

    size_t          m_tx_size;       //! TX buffer size.
    uint8_t         *m_rx;           //! Receive buffer.
    size_t          m_rx_size;       //! RX buffer size.
    uint8_t         m_status;        //! Represents bus status.
};

template<spi_device dev>
spi_i2s_bus<dev>::spi_i2s_bus()
    :m_event_handler{}
    ,m_tx{nullptr}
    ,m_tx_size{0}
    ,m_rx{nullptr}
    ,m_rx_size{0}
    ,m_status{0}
{

}

template<spi_device dev>
spi_i2s_bus<dev>::~spi_i2s_bus()
{

}

template<spi_device dev>
ecl::err spi_i2s_bus<dev>::init()
{
    if (m_status & inited) {
        return ecl::err::ok;
    }

    constexpr auto spi = pick_spi();
    constexpr auto rcc_periph = pick_rcc();
    constexpr auto rcc_fn = pick_rcc_fn();

    rcc_fn(rcc_periph, ENABLE);

    init_dma();

    init_interface();

    m_status |= inited;

    return ecl::err::ok;
}

template<spi_device dev>
void spi_i2s_bus<dev>::set_rx(uint8_t *rx, size_t size)
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

template<spi_device dev>
void spi_i2s_bus<dev>::set_tx(size_t size, uint8_t fill_byte)
{
    if (!(m_status & inited)) {
        return;
    }

    m_status |= mode_fill;
    m_status &= ~(tx_hidden);
    m_tx.byte = fill_byte;
    m_tx_size = size;
}

template<spi_device dev>
void spi_i2s_bus<dev>::set_tx(const uint8_t *tx, size_t size)
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

    m_status &= ~(mode_fill);
    m_status &= ~(tx_hidden);
    m_tx.buf = tx;
    m_tx_size = size;
}


template<spi_device dev>
void spi_i2s_bus<dev>::set_handler(const handler_fn &handler)
{
    m_event_handler = handler;
}


template<spi_device dev>
void spi_i2s_bus<dev>::reset_buffers()
{
    m_status &= ~(mode_fill);
    m_tx.buf = nullptr;
    m_tx_size = 0;
    m_rx = nullptr;
    m_rx_size = 0;
}

template<spi_device dev>
void spi_i2s_bus<dev>::reset_handler()
{
    m_event_handler = handler_fn{};
}

template<spi_device dev>
ecl::err spi_i2s_bus<dev>::do_xfer()
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

template<spi_device dev>
bool spi_i2s_bus<dev>::valid_sizes()
{
    // Bus is in full-duplex mode. Different sizes are not permitted.
    if (m_tx_size && m_rx_size) {
        if (m_tx_size != m_rx_size) {
            return false;
        }
    }

    return true;
}

template<spi_device dev>
void spi_i2s_bus<dev>::prepare_tx()
{
    if (m_status & tx_hidden) {
        // TX is not requested by user but it is required to setup this stream
        // in SPI in order to receive data from RX channel.
        // Trick is to use fill mode in TX channel without notifying
        // user about it.

        if (!m_rx_size) {
            // TODO: is this correct that both buffers with 0 size???
            for (;;);
        }

        m_status |= mode_fill;
        m_tx_size = m_rx_size;
        m_tx.byte = 0xff;
    }

    m_status &= ~(tx_complete);

    auto tx_dma = dma::get_stream<config::dma::tx_stream>();
    constexpr auto spi = pick_spi();

    DMA_InitTypeDef dma_init;
    DMA_StructInit(&dma_init);

    dma_init.DMA_Channel            = config::dma::tx_channel;
    dma_init.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    dma_init.DMA_PeripheralBaseAddr = reinterpret_cast< uint32_t >(&spi->DR);
    dma_init.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;

    if (m_status & mode_fill) {
        dma_init.DMA_MemoryInc       = DMA_MemoryInc_Disable;
        dma_init.DMA_Memory0BaseAddr = reinterpret_cast< uint32_t >(&m_tx.byte);
    } else {
        dma_init.DMA_MemoryInc       = DMA_MemoryInc_Enable;
        dma_init.DMA_Memory0BaseAddr = reinterpret_cast< uint32_t >(m_tx.buf);
    }

    if (config::bus_type == spi_bus_type::i2s) {
        dma_init.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
        dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        dma_init.DMA_BufferSize         = m_tx_size / 2;
    } else {
        dma_init.DMA_BufferSize         = m_tx_size;
    }

    DMA_Init(tx_dma, &dma_init);
    DMA_ITConfig(tx_dma, DMA_IT_TC | DMA_IT_HT, ENABLE);
}

template<spi_device dev>
void spi_i2s_bus<dev>::prepare_rx()
{
    if (!m_rx_size) {
        // Preventing event routine from waiting RX complete event.
        m_status |= rx_complete;
        return;
    }

    m_status &= ~(rx_complete);

    auto rx_dma = dma::get_stream<config::dma::rx_stream>();
    constexpr auto spi = pick_spi();

    DMA_InitTypeDef dma_init;
    DMA_StructInit(&dma_init);

    dma_init.DMA_Channel            = config::dma::rx_channel;
    dma_init.DMA_DIR                = DMA_DIR_PeripheralToMemory;
    dma_init.DMA_PeripheralBaseAddr = reinterpret_cast< uint32_t >(&spi->DR);
    dma_init.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dma_init.DMA_Memory0BaseAddr    = reinterpret_cast< uint32_t >(m_rx);

    if (config::bus_type == spi_bus_type::i2s) {
        dma_init.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
        dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        dma_init.DMA_BufferSize         = m_rx_size / 2;
    } else {
        dma_init.DMA_BufferSize = m_rx_size;
    }

    DMA_Init(rx_dma, &dma_init);
    DMA_ITConfig(rx_dma, DMA_IT_TC | DMA_IT_HT, ENABLE);
}

template<spi_device dev>
void spi_i2s_bus<dev>::start_xfer()
{
    auto rx_dma = dma::get_stream<config::dma::rx_stream>();
    auto tx_dma = dma::get_stream<config::dma::tx_stream>();
    constexpr auto spi = pick_spi();

    // After all directions configured, streams may be enabled
    if (m_tx_size) {
        DMA_Cmd(tx_dma, ENABLE);
    }

    if (m_rx_size) {
        DMA_Cmd(rx_dma, ENABLE);
    }

    // Enable interrupt request from SPI periphery
    SPI_I2S_DMACmd(spi, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);
}

template<spi_device dev>
void spi_i2s_bus<dev>::irq_handler()
{
    constexpr auto rx_irqn = dma::get_irqn<config::dma::rx_stream>();
    constexpr auto tx_irqn = dma::get_irqn<config::dma::tx_stream>();

    auto tx_dma = dma::get_stream<config::dma::tx_stream>();
    auto rx_dma = dma::get_stream<config::dma::rx_stream>();

    constexpr auto tx_tc_if = dma::get_tc_if<config::dma::tx_stream>();
    constexpr auto rx_tc_if = dma::get_tc_if<config::dma::rx_stream>();

    constexpr auto tx_ht_if = dma::get_ht_if<config::dma::tx_stream>();
    constexpr auto rx_ht_if = dma::get_ht_if<config::dma::rx_stream>();

    constexpr auto spi = pick_spi();

    if (!(m_status & tx_complete)) {
        if (DMA_GetITStatus(tx_dma, tx_tc_if)) {
            // Complete TX transaction
            constexpr auto tx_tc_flag = dma::get_tc_flag<config::dma::tx_stream>();

            if (!(m_status & tx_hidden)) {
                m_event_handler(channel::tx, event::tc, m_tx_size);
            }

            DMA_ClearFlag(tx_dma, tx_tc_flag);
            DMA_ClearITPendingBit(tx_dma, tx_tc_if);
            DMA_ITConfig(tx_dma, DMA_IT_TC | DMA_IT_HT, DISABLE);

            m_status |= tx_complete;
        } else if (DMA_GetITStatus(tx_dma, tx_ht_if)) {
            constexpr auto tx_ht_flag = dma::get_ht_flag<config::dma::tx_stream>();

            uint32_t tx_left = DMA_GetCurrDataCounter(tx_dma);

            if (config::bus_type == spi_bus_type::i2s) {
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
            constexpr auto rx_tc_flag = dma::get_tc_flag<config::dma::rx_stream>();

            m_event_handler(channel::rx, event::tc, m_rx_size);

            DMA_ClearFlag(rx_dma, rx_tc_flag);
            DMA_ClearITPendingBit(rx_dma, rx_tc_if);
            DMA_ITConfig(rx_dma, DMA_IT_TC | DMA_IT_HT, DISABLE);

            m_status |= rx_complete;
        } else if (DMA_GetITStatus(rx_dma, rx_ht_if)) {
            constexpr auto rx_ht_flag = dma::get_ht_flag<config::dma::tx_stream>();

            uint32_t rx_left = DMA_GetCurrDataCounter(rx_dma);

            if (config::bus_type == spi_bus_type::i2s) {
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

template<spi_device dev>
constexpr auto spi_i2s_bus<dev>::pick_spi()
{
    switch (dev) {
        case spi_device::bus1:
            return SPI1;
        case spi_device::bus2:
            return SPI2;
        case spi_device::bus3:
            return SPI3;
        case spi_device::bus4:
            return SPI4;
        case spi_device::bus5:
            return SPI5;
        case spi_device::bus6:
            return SPI6;
        default:
            // TODO: clarify
            return static_cast< decltype(SPI1) >(nullptr);
    }
}

template<spi_device dev>
constexpr auto spi_i2s_bus<dev>::pick_rcc()
{
    // TODO: comments
    switch (dev) {
        case spi_device::bus1:
            return RCC_APB2Periph_SPI1;
        case spi_device::bus2:
            return RCC_APB1Periph_SPI2;
        case spi_device::bus3:
            return RCC_APB1Periph_SPI3;
        case spi_device::bus4:
            return RCC_APB2Periph_SPI4;
        case spi_device::bus5:
            return RCC_APB2Periph_SPI5;
        case spi_device::bus6:
            return RCC_APB2Periph_SPI6;
        default:
            // TODO: clarify
            return static_cast< decltype(RCC_APB2Periph_SPI6) >(-1);
    }
}

template<spi_device dev>
constexpr auto spi_i2s_bus<dev>::pick_rcc_fn()
{
    // APB1 - SPI3 SPI2
    // APB2 - SPI5 SPI6 SPI1 SPI4
    switch (dev) {
        case spi_device::bus1:
        case spi_device::bus5:
        case spi_device::bus4:
        case spi_device::bus6:
            return RCC_APB2PeriphClockCmd;
        case spi_device::bus2:
        case spi_device::bus3:
            return RCC_APB1PeriphClockCmd;
        default:
            // TODO: clarify
            return static_cast< decltype(&RCC_APB2PeriphClockCmd) >(nullptr);
    }
}

template<spi_device dev>
constexpr uint32_t
spi_i2s_bus<dev>::pick_i2s_audio_frequency(ecl::i2s::audio_frequency value)
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

template<spi_device dev>
constexpr uint32_t
spi_i2s_bus<dev>::pick_i2s_data_format(ecl::i2s::data_format value)
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

template<spi_device dev>
template<class Iface_cfg>
std::enable_if_t<Iface_cfg::bus_type == spi_bus_type::i2s, void>
spi_i2s_bus<dev>::init_interface()
{
    RCC_PLLI2SCmd(ENABLE);

    constexpr auto i2s = pick_spi();
    auto init_obj = spi_i2s_cfg<dev>::init_obj;
    I2S_Init(i2s, &init_obj);

    // TODO: disable I2S when there in no XFER
    I2S_Cmd(i2s, ENABLE);

}

template<spi_device dev>
template<class Iface_cfg>
std::enable_if_t<Iface_cfg::bus_type == spi_bus_type::spi, void>
spi_i2s_bus<dev>::init_interface()
{
    constexpr auto spi = pick_spi();
    auto init_obj = spi_i2s_cfg<dev>::init_obj;
    SPI_Init(spi, &init_obj);

    // TODO: disable SPI when there in no XFER
    SPI_Cmd(spi, ENABLE);
}

template<spi_device dev>
void spi_i2s_bus<dev>::init_dma()
{
    dma::init_rcc<config::dma::rx_stream>();
    dma::init_rcc<config::dma::tx_stream>();

    auto handler = [this]() {
        this->irq_handler();
    };

    dma::subscribe_irq<config::dma::rx_stream>(handler);
    dma::subscribe_irq<config::dma::tx_stream>(handler);
}

template<spi_device dev>
template<ecl::i2s::audio_frequency frequency, class Iface_cfg>
std::enable_if_t<Iface_cfg::bus_type == spi_bus_type::i2s, ecl::err>
spi_i2s_bus<dev>::i2s_set_audio_frequency()
{
    static_assert(frequency != ecl::i2s::audio_frequency::k88,
                  "frequency is not supported by a platform");

    return i2s_set_audio_frequency_private(pick_i2s_audio_frequency(frequency));
}

template<spi_device dev>
ecl::err spi_i2s_bus<dev>::i2s_set_audio_frequency_private(uint32_t value)
{
    constexpr auto spi = pick_spi();
    // TODO check if spi is inited

    // Disable I2S before changing any parameters according to RM
    I2S_Cmd(spi, DISABLE);

    // There is a complex procedure of setting correct
    // prescaler to achieve necessary frequency.
    // It is done in I2S_Init(), so we can reuse it.

    // Can be a little confusing, we cannot use i2s_init = config::init_obj,
    // since config::init_obj is calculated in compile time, so the structure
    // itself is not created. Linker is very upset about it.
    I2S_InitTypeDef i2s_init;
    i2s_init.I2S_Standard   = config::init_obj.I2S_DataFormat;
    i2s_init.I2S_CPOL       = config::init_obj.I2S_CPOL;
    i2s_init.I2S_MCLKOutput = config::init_obj.I2S_MCLKOutput;
    i2s_init.I2S_Mode       = config::init_obj.I2S_Mode;

    // Data format can be changed by i2s_set_data_format()
    i2s_init.I2S_DataFormat = spi->I2SCFGR & SPI_I2SCFGR_DATLEN;
    // Change audio frequency
    i2s_init.I2S_AudioFreq = value;

    // Re-init I2S with new audio frequency
    I2S_Init(spi, &i2s_init);

    I2S_Cmd(spi, ENABLE);

    return ecl::err::ok;
}

template<spi_device dev>
template<ecl::i2s::data_format format, class Iface_cfg>
std::enable_if_t<Iface_cfg::bus_type == spi_bus_type::i2s, ecl::err>
spi_i2s_bus<dev>::i2s_set_data_format()
{
    static_assert(format != ecl::i2s::data_format::b8,
                  "format is not supported by a platform");

    return i2s_set_data_format_private(pick_i2s_data_format(format));
}

template<spi_device dev>
ecl::err spi_i2s_bus<dev>::i2s_set_data_format_private(uint32_t value)
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
