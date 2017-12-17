/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup stm32 STM32 multi-platform
//! @{

//! \defgroup stm32_spi_i2s I2S and SPI driver
//! @{

//! \file
//! \brief Contains platform driver implementation for SPI and I2S buses.
//! \details In STM32F4 series SPI and I2S are implemented within the same controller.
#ifndef PLATFORM_SPI_BUS_HPP_
#define PLATFORM_SPI_BUS_HPP_

#include <stm32_device.hpp>

#include <common/spi.hpp>
#include <common/bus.hpp>
#include <common/irq.hpp>

#include <sys/types.h>
#include <ecl/assert.h>

#include <common/console.hpp>

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
//! template specialization for required SPI/I2S device. Since STM32 devices
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
//! - **dma_tx** alias - Specifies DMA TX wrapper. Refer to the implementation
//!     for the particular STM32 family to get insight how to properly define it.
//! - **dma_rx** alias - Specifies DMA RX wrapper. Refer to the implementation
//!     for the particular STM32 family to get insight how to properly define it.
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
//! \par SPI configuration example for STM32F4XX.
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
//!     using dma_tx = ecl::dma_wrap<ecl::dma_stream::dma1_4, ecl::dma_channel::ch0>;
//!     using dma_rx = ecl::dma_wrap<ecl::dma_stream::dma1_3, ecl::dma_channel::ch0>;
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
//!     using dma_tx = ecl::dma_wrap<ecl::dma_stream::dma1_4, ecl::dma_channel::ch0>;
//!     using dma_rx = ecl::dma_wrap<ecl::dma_stream::dma1_3, ecl::dma_channel::ch0>;
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
//! Thus, redefinition of the config class for given SPI will result in
//! compilation errors. *Good practice is to place all SPI configuration
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

    // Static use only
    spi_i2s_bus() = delete;
    ~spi_i2s_bus() = delete;

    //! Lazy initialization.
    //! \return Status of operation.
    static ecl::err init();

    //! Sets rx buffer with given size.
    //! \param[in,out]  rx      Buffer to write data to. Optional.
    //! \param[in]      size    Size
    static void set_rx(uint8_t *rx, size_t size);

    //! Sets tx buffer made-up from sequence of similar bytes.
    //! \param[in] size         Size of sequence
    //! \param[in] fill_byte    Byte to fill a sequence. Optional.
    static void set_tx(size_t size, uint8_t fill_byte = 0xff);

    //! Sets tx buffer with given size.
    //! \param[in] tx   Buffer to transmit. Optional.
    //! \param[in] size Buffer size.
    static void set_tx(const uint8_t *tx, size_t size);

    //! Sets event handler.
    //! \details Handler will be used by the bus, until reset_handler() will be called.
    //! \param[in] handler Handler itself.
    static void set_handler(const handler_fn &handler);

    //! Reset xfer buffers.
    //! \details Buffers that were set by \sa set_tx() and \sa set_rx()
    //! will be no longer used after this call.
    static void reset_buffers();

    //! Resets previously set handler.
    static void reset_handler();

    //! Executes xfer, using buffers previously set.
    //! \details When it will be done, handler will be invoked.
    //! \return Status of operation.
    static ecl::err do_xfer();

    //! Sets audio frequency for I2S mode.
    //! \details Is not used in SPI mode.
    //! \tparam frequency Can be a type of ecl::i2s::audio_frequency
    //! \tparam Iface_cfg Used for correct enable_if mechanism. Must not be passed by user.
    //! \return Status of operation.
    template<ecl::i2s::audio_frequency frequency, class Iface_cfg = config>
    static std::enable_if_t<Iface_cfg::bus_type == spi_bus_type::i2s, ecl::err>
    i2s_set_audio_frequency();

    //! Sets data format for I2S mode.
    //! \details Is not used in SPI mode.
    //! \tparam format Can be a type of ecl::i2s::data_format
    //! \tparam Iface_cfg Used for correct enable_if mechanism. Must not be passed by user.
    //! \return Status of operation.
    template<ecl::i2s::data_format format, class Iface_cfg = config>
    static std::enable_if_t<Iface_cfg::bus_type == spi_bus_type::i2s, ecl::err>
    i2s_set_data_format();

    //! Enables circular mode for the bus.
    //! \details In circular mode when xfer is finished, the new xfer is started
    //! automatically without interruption. The same buffer is used.
    //! The process continues until circular mode is disabled.
    //! This method must not be called during xfer().
    static void enable_circular_mode();

    //! Disables circular mode for the bus.
    //! \details This method allows to stop xfer() that was started with circular mode.
    //! It is recommended to call this method in event handler during processing of the HT or TC event.
    //! The xfer is always stopped after TC.
    //! If circular mode is disabled during processing of the TC (or between HT and TC),
    //! the xfer will be stopped immediately after current TC will be processed.
    //! If circular mode is disabled during processing of the HT (or between TC and HT),
    //! the xfer will be finished after next TC will be processed.
    //! This method also can be called when xfer() is not active.
    static void disable_circular_mode();

    //! Returns the circular mode state.
    //! \return true if circular mode is enabled, false otherwise
    static bool is_circular_mode();

private:
    static constexpr auto pick_spi();
    static constexpr auto pick_spi_irqn();
    static constexpr auto pick_rcc();
    static constexpr auto pick_rcc_fn();

    static constexpr uint32_t
    pick_i2s_audio_frequency(ecl::i2s::audio_frequency value);
    static constexpr uint32_t
    pick_i2s_data_format(ecl::i2s::data_format value);

    static ecl::err i2s_set_audio_frequency_private(uint32_t value);
    static ecl::err i2s_set_data_format_private(uint32_t value);

    //! Initiliazes I2S interface, if bus was configured is such way
    template<class Iface_cfg = config>
    static std::enable_if_t<Iface_cfg::bus_type == spi_bus_type::i2s, void>
    init_interface();

    //! Initiliazes SPI interface, if bus was configured is such way
    template<class Iface_cfg = config>
    static std::enable_if_t<Iface_cfg::bus_type == spi_bus_type::spi, void>
    init_interface();

    // DMA init helper
    static void init_dma();

    // Checks if buffer sizes are valid
    static bool valid_sizes();

    // Prepares TX transaction, if needed
    static void prepare_tx();

    // Prepares RX transaction, if needed
    static void prepare_rx();

    // Starts transaction, if needed
    static void start_xfer();

    // RCC init helper
    static void init_rcc();

    // IRQ init helper
    static void init_irq();

    // Handles DMA events
    // TODO Implement IRQ mode #102
    static void irq_handler();

    //! Bus is inited if this flag is set.
    static constexpr uint8_t inited        = 0x1;
    //! Bus is in fill mode if this flag is set.
    static constexpr uint8_t mode_fill     = 0x2;
    //! Bus DMA is in circular mode if this flag is set.
    static constexpr uint8_t mode_circular = 0x4;
    //! TX is finished if this flag is set.
    static constexpr uint8_t tx_complete   = 0x8;
    //! User will not be notified about TX events if this flag is set.
    static constexpr uint8_t tx_hidden     = 0x10;
    //! RX is finished if this flag is set.
    static constexpr uint8_t rx_complete   = 0x20;
    //! User will not be notified about RX events if this flag is set.
    static constexpr uint8_t rx_hidden     = 0x40;

    static union tx_data
    {
        const uint8_t *buf;         //! Transmit buffer.
        uint16_t      byte;         //! Byte to transmit (16 bits of 16-bit SPI mode)
    } m_tx;

    static size_t  m_tx_size;       //! TX buffer size.
    static uint8_t *m_rx;           //! Receive buffer.
    static size_t  m_rx_size;       //! RX buffer size.
    static uint8_t m_status;        //! Represents bus status.

    //! Storage type for the event handler.
    using handler_storage = std::aligned_storage_t<sizeof(handler_fn), alignof(handler_fn)>;

    //! Storage for the event handler.
    static handler_storage m_handler_storage;

    //! Gets event handler
    static constexpr auto &get_handler() { return reinterpret_cast<handler_fn&>(m_handler_storage); }
};

template<spi_device dev>
size_t spi_i2s_bus<dev>::m_tx_size;

template<spi_device dev>
size_t spi_i2s_bus<dev>::m_rx_size;

template<spi_device dev>
typename spi_i2s_bus<dev>::tx_data spi_i2s_bus<dev>::m_tx;

template<spi_device dev>
uint8_t *spi_i2s_bus<dev>::m_rx;

template<spi_device dev>
uint8_t spi_i2s_bus<dev>::m_status = spi_i2s_bus<dev>::tx_complete | spi_i2s_bus<dev>::rx_complete;

template<spi_device dev>
typename spi_i2s_bus<dev>::handler_storage spi_i2s_bus<dev>::m_handler_storage;

template<spi_device dev>
ecl::err spi_i2s_bus<dev>::init()
{
    if (m_status & inited) {
        return ecl::err::ok;
    }

    new (&m_handler_storage) handler_fn;

    constexpr auto spi_irq = pick_spi_irqn();
    constexpr auto spi     = pick_spi();

    init_rcc();
    init_irq();

    init_interface();

    // Provide basic error handling
    // TODO #121 - implement proper error handling

    ecl::irq::subscribe(spi_irq,
                        [] { ecl_assert_msg(0, "SPI bus failure occur"); });
    SPI_I2S_ITConfig(spi, SPI_I2S_IT_ERR, ENABLE);

    m_status |= inited;

    return ecl::err::ok;
}

template<spi_device dev>
void spi_i2s_bus<dev>::set_rx(uint8_t *rx, size_t size)
{
    if (!(m_status & inited)) {
        return;
    }

    // Treat nullptr buffers as indication that xfer of this type
    // is not required
    if (!rx) {
        // TX channel must be enabled in bidir SPI even if user don't
        // want it. SPI IRQ handler must not propagate RX events in this case.
        m_status |= rx_hidden;
        return;
    }

    m_rx      = rx;
    m_rx_size = size;
    m_status &= ~(rx_hidden);
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

    m_status &= ~(mode_fill);

    // Treat nullptr buffers as indication that xfer of this type
    // is not required
    if (!tx) {
        // TX channel must be enabled in bidir SPI even if user don't
        // want it.
        m_status |= tx_hidden;
        return;
    }

    m_status &= ~(tx_hidden);
    m_tx.buf = tx;
    m_tx_size = size;
}


template<spi_device dev>
void spi_i2s_bus<dev>::set_handler(const handler_fn &handler)
{
    get_handler() = handler;
}


template<spi_device dev>
void spi_i2s_bus<dev>::reset_buffers()
{
    m_status &= ~(mode_fill);
    m_tx.buf = nullptr;
    m_tx_size = 0;
    m_rx      = nullptr;
    m_rx_size = 0;
}

template<spi_device dev>
void spi_i2s_bus<dev>::reset_handler()
{
    get_handler() = handler_fn{};
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

        // TODO: is this correct that both buffers have 0 size???
        ecl_assert(m_rx_size);

        m_status |= mode_fill;
        m_tx_size = m_rx_size;
        m_tx.byte = 0xff;
    }

    m_status &= ~(tx_complete);

    constexpr auto spi     = pick_spi();
    constexpr auto data_sz = config::bus_type == spi_bus_type::i2s ?
                             dma_data_sz::hword : dma_data_sz::byte;

    if (m_status & mode_fill) {
        config::dma_tx::template mem_to_periph<data_sz>(m_tx.byte, m_tx_size,
                                                        &spi->DR);
    } else {
        if (is_circular_mode()) {
            config::dma_tx::template mem_to_periph<data_sz, dma_mode::circular>(
                    m_tx.buf, m_tx_size, &spi->DR);
        } else {
            config::dma_tx::template mem_to_periph<data_sz>(m_tx.buf, m_tx_size,
                                                            &spi->DR);
        }
    }

    config::dma_tx::template enable_events_irq();
}

template<spi_device dev>
void spi_i2s_bus<dev>::prepare_rx()
{
    constexpr auto spi = pick_spi();
    m_status &= ~(rx_complete);

    constexpr auto data_sz = config::bus_type == spi_bus_type::i2s ?
                             dma_data_sz::hword : dma_data_sz::byte;

    if (m_status & rx_hidden) {
        m_rx_size = m_tx_size; // Informs start_xfer() that RX DMA must be enabled
        config::dma_rx::template periph_to_mem<dma_data_sz::byte>(&spi->DR,
                                                                  m_rx_size);
    } else {
        config::dma_rx::template periph_to_mem<data_sz>(&spi->DR, m_rx,
                                                        m_rx_size);
    }

    config::dma_rx::template enable_events_irq();
}

template<spi_device dev>
void spi_i2s_bus<dev>::start_xfer()
{
    constexpr auto spi = pick_spi();

    // After all directions configured, streams may be enabled
    if (m_tx_size) {
        config::dma_tx::enable();
    }

    if (m_rx_size) {
        config::dma_rx::enable();
    }

    // Enable interrupt request from SPI periphery
    SPI_I2S_DMACmd(spi, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);
}

template<spi_device dev>
void spi_i2s_bus<dev>::irq_handler()
{
    constexpr auto rx_irqn = config::dma_rx::get_irqn();
    constexpr auto tx_irqn = config::dma_tx::get_irqn();

    constexpr auto spi = pick_spi();

    // Basic DMA error handling
    // TODO #121 - implement proper error handling

    ecl_assert(!config::dma_rx::err());
    ecl_assert(!config::dma_tx::err());

    if (!(m_status & tx_complete)) {
        bool tx_irq_served = false;

        if (config::dma_tx::ht()) {
            uint32_t tx_left = config::dma_tx::bytes_left();

            if (!(m_status & tx_hidden)) {
                get_handler()(channel::tx, event::ht, m_tx_size - tx_left);
            }

            config::dma_tx::clear_ht();
            tx_irq_served = true;
        }

        if (config::dma_tx::tc()) {
            // Complete TX transaction

            if (!(m_status & tx_hidden)) {
                get_handler()(channel::tx, event::tc, m_tx_size);
            }

            config::dma_tx::clear_tc();

            if (!is_circular_mode()) {
                config::dma_tx::template disable_events_irq();
                m_status |= tx_complete;
            }

            tx_irq_served = true;
        }

        if (tx_irq_served) {
            irq::clear(tx_irqn);
            irq::unmask(tx_irqn);
        }
    }

    if (!(m_status & rx_complete)) {
        bool rx_irq_served = false;

        if (config::dma_rx::ht()) {
            uint32_t rx_left = config::dma_rx::bytes_left();

            if (!(m_status & rx_hidden)) {
                get_handler()(channel::rx, event::ht, m_rx_size - rx_left);
            }

            config::dma_rx::clear_ht();
            rx_irq_served = true;
        }

        if (config::dma_rx::tc()) {
            // Complete RX transaction
            if (!(m_status & rx_hidden)) {
                get_handler()(channel::rx, event::tc, m_rx_size);
            }

            config::dma_rx::clear_tc();

            if (!is_circular_mode()) {
                config::dma_rx::template disable_events_irq();
                m_status |= rx_complete;
            }

            rx_irq_served = true;
        }

        if (rx_irq_served) {
            irq::clear(rx_irqn);
            irq::unmask(rx_irqn);
        }
    }

    if ((m_status & (rx_complete | tx_complete)) ==
        (rx_complete | tx_complete)) {
        // All transfers completed
        get_handler()(channel::meta, event::tc, m_tx_size);

        if (!is_circular_mode()) {
            config::dma_rx::disable();
            config::dma_tx::disable();

            SPI_I2S_DMACmd(spi, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);
        }

        // Clear/enable NVIC interrupts
        irq::clear(rx_irqn);
        irq::unmask(rx_irqn);
        irq::clear(tx_irqn);
        irq::unmask(tx_irqn);
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
#if CONFIG_SPI_COUNT > 3
        case spi_device::bus4:
            return SPI4;
#endif
#if CONFIG_SPI_COUNT > 4
        case spi_device::bus5:
            return SPI5;
#endif
#if CONFIG_SPI_COUNT > 5
        case spi_device::bus6:
            return SPI6;
#endif
    }
}

template<spi_device dev>
constexpr auto spi_i2s_bus<dev>::pick_spi_irqn()
{
    switch (dev) {
        case spi_device::bus1:
            return SPI1_IRQn;
        case spi_device::bus2:
            return SPI2_IRQn;
        case spi_device::bus3:
            return SPI3_IRQn;
#if CONFIG_SPI_COUNT > 3
        case spi_device::bus4:
            return SPI4_IRQn;
#endif
#if CONFIG_SPI_COUNT > 4
        case spi_device::bus5:
            return SPI5_IRQn;
#endif
#if CONFIG_SPI_COUNT > 5
        case spi_device::bus6:
            return SPI6_IRQn;
#endif
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
#if CONFIG_SPI_COUNT > 3
        case spi_device::bus4:
            return RCC_APB2Periph_SPI4;
#endif
#if CONFIG_SPI_COUNT > 4
        case spi_device::bus5:
            return RCC_APB2Periph_SPI5;
#endif
#if CONFIG_SPI_COUNT > 5
        case spi_device::bus6:
            return RCC_APB2Periph_SPI6;
#endif
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
void spi_i2s_bus<dev>::init_rcc()
{
    constexpr auto rcc_periph = pick_rcc();
    constexpr auto rcc_fn     = pick_rcc_fn();

    rcc_fn(rcc_periph, ENABLE);

    config::dma_tx::init();
    config::dma_rx::init();
}

template<spi_device dev>
void spi_i2s_bus<dev>::init_irq()
{
    auto handler = []() {
        irq_handler();
    };

    // TODO: FIXME: CODE DUPLICATION!!!!!!

    constexpr auto dma_irqn_rx = config::dma_rx::get_irqn();
    constexpr auto dma_irqn_tx = config::dma_tx::get_irqn();

    // Prevent spurious interrupts from occurrence
    ecl::irq::mask(dma_irqn_rx);
    ecl::irq::mask(dma_irqn_tx);

    // Do not expose old, not yet handled interrupts
    ecl::irq::clear(dma_irqn_rx);
    ecl::irq::clear(dma_irqn_tx);

    // Subscribe, actually
    ecl::irq::subscribe(dma_irqn_rx, handler);
    ecl::irq::subscribe(dma_irqn_tx, handler);

    // Go on
    // TODO: think about letting user to decide when to unmask interrupts
    ecl::irq::unmask(dma_irqn_rx);
    ecl::irq::unmask(dma_irqn_tx);
}

template<spi_device dev>
template<class Iface_cfg>
std::enable_if_t<Iface_cfg::bus_type == spi_bus_type::i2s, void>
spi_i2s_bus<dev>::init_interface()
{
#ifdef stm32f4xx
    RCC_PLLI2SCmd(ENABLE);
#endif

    constexpr auto i2s       = pick_spi();
    constexpr auto cinit_obj = spi_i2s_cfg<dev>::init_obj;
    auto           init_obj  = cinit_obj;
    I2S_Init(i2s, &init_obj);

    // TODO: disable I2S when there in no XFER
    I2S_Cmd(i2s, ENABLE);
}

template<spi_device dev>
template<class Iface_cfg>
std::enable_if_t<Iface_cfg::bus_type == spi_bus_type::spi, void>
spi_i2s_bus<dev>::init_interface()
{
    constexpr auto spi       = pick_spi();
    constexpr auto cinit_obj = spi_i2s_cfg<dev>::init_obj;
    auto           init_obj  = cinit_obj;
    SPI_Init(spi, &init_obj);

    // TODO: disable SPI when there in no XFER
    SPI_Cmd(spi, ENABLE);
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
    i2s_init.I2S_AudioFreq  = value;

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

template<spi_device dev>
void spi_i2s_bus<dev>::enable_circular_mode()
{
    // Cannot enable circular mode if transfer is ongoing
    ecl_assert((m_status & tx_complete) && (m_status & rx_complete));

    m_status |= mode_circular;
}

template<spi_device dev>
void spi_i2s_bus<dev>::disable_circular_mode()
{
    m_status &= ~mode_circular;
}

template<spi_device dev>
bool spi_i2s_bus<dev>::is_circular_mode()
{
    return m_status & mode_circular;
}

} // namespace ecl

#endif // PLATFORM_SPI_BUS_HPP_

//! @}

//! @}

//! @}
