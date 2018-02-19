/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Contains platform driver skeleton for SPI bus.
//! \details This file can be used when creating new platform to speedup
//! SPI driver development.

#ifndef PLATFORM_SPI_HPP_
#define PLATFORM_SPI_HPP_

#include <ecl/assert.h>
#include <common/bus.hpp>

namespace ecl
{

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup skel Platform skeleton
//! @{

//! \defgroup skel_spi Skeleton of SPI driver
//! @{

//! List of avaliable SPI channels.
enum class spi_channel
{
    ch1,
    ch2,
    ch3,
};

//! SPI master/slave selection.
enum class spi_type
{
    master,
    // slave  - TODO: support slave mode
};

//! SPI clock polarity
enum class spi_cpol
{
    low,  //! CPOL = 0
    high, //! CPOL = 1
};

//! SPI clock phase
enum class spi_cpha
{
    low,  //! CPHA = 0
    high, //! CPHA = 1
};

//! Base template class for the SPI configuration
//! \details In order to advertise configuration parameters user must create
//! template specialization for required SPI device.
//!
//! SPI-specific fields must be present:
//!  - SPI type: master/slave \sa ecl::spi_type
//!  - SPI clock polarity \sa ecl::cpol
//!  - SPI clock phase \sa ecl::cpha
//!
//! \par SPI configuration example.
//! In order to use this configuration class one must create configuration class
//! in the `ecl` namespace before any access to SPI instance.
//!
//! \code{.cpp}
//! namespace ecl
//! {
//!
//! template<>
//! struct spi_cfg<spi_channel::ch1>
//! {
//!     static constexpr auto type = spi_type::master;
//!     static constexpr auto cpol = spi_cpol::high;
//!     static constexpr auto cpha = spi_cpha::low;
//! };
//!
//! } // namespace ecl
//! \endcode
//!
//! \warning To avoid potential problems with multiple configurations for single
//! SPI bus, **make sure that full specialization is placed in the
//! header included (directly or indirectly) by all dependent modules.**.
//! Thus, redefinition of the config class for given SPI will result in
//! compilation errors. *Good practice is to place all SPI configuration
//! class in the single target-related header.*
template<spi_channel ch>
struct spi_cfg
{
    // Always assert
    static_assert(std::is_integral<decltype(ch)>::value,
                  "The instance of this generic class should never be "
                          "instantiated. Please write your own template specialization "
                          "of this class. See documentation.");
};


//! \brief Driver implementation for SPI
//! \tparam ch SPI configuration
//!
template<spi_channel ch>
class spi
{
public:
    // Convenient type aliases.
    using channel       = bus_channel;
    using event         = bus_event;
    using handler_fn    = bus_handler;
    using config        = spi_cfg<ch>;

    // Static use only
    spi() = delete;
    ~spi() = delete;

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
    //! \details When xfer is done, handler will be invoked.
    //! \return Status of operation.
    static ecl::err do_xfer();

    //! Enables circular mode for the bus.
    //! \details In circular mode when xfer is finished, the new xfer is started
    //! automatically without interruption. The same buffer is used.
    //! The process continues until circular mode is disabled.
    //! This method must not be called during xfer().
    static void enable_circular_mode();

    //! Disables circular mode for the bus.
    //! \details This method allows to stop xfer() that was started with circular mode.
    //! It is recommended to call this method in event handler during processing
    //! of the HT or TC event. The xfer is always stopped after TC.
    //! If circular mode is disabled during processing of the TC
    //! (or between HT and TC), the xfer will be stopped immediately after
    //! current TC will be processed.
    //! If circular mode is disabled during processing of the HT
    //! (or between TC and HT), the xfer will be finished after next TC
    //! will be processed.
    //! This method also can be called when xfer() is not active.
    static void disable_circular_mode();

    //! Returns the circular mode state.
    //! \return true if circular mode is enabled, false otherwise
    static bool is_circular_mode();

protected:
    //! SPI interrupt handler.
    //! \details Receives interrupt from IRQ subsystem.
    static void irq_handler();

private:
    // Checks if buffer sizes are valid
    static bool valid_sizes();

    //! Bus is inited if this flag is set.
    static constexpr uint8_t inited        = 0x1;
    //! Bus is in fill mode if this flag is set.
    static constexpr uint8_t mode_fill     = 0x2;
    //! Bus DMA is in circular mode if this flag is set.
    static constexpr uint8_t mode_circular = 0x4;
    //! TX is finished if this flag is set.
    static constexpr uint8_t tx_complete   = 0x8;
    //! User will not be notified about TX events if this flag is set.
    //! \details This flag is required to prevent event propagation in
    //! case if RX stream must be ignored.
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

    //! Storage for the event handler.
    static safe_storage<handler_fn> m_handler_storage;

    //! Gets event handler
    static constexpr auto &get_handler() { return m_handler_storage.get(); }
};

template<spi_channel ch>
size_t spi<ch>::m_tx_size;

template<spi_channel ch>
size_t spi<ch>::m_rx_size;

template<spi_channel ch>
typename spi<ch>::tx_data spi<ch>::m_tx;

template<spi_channel ch>
uint8_t *spi<ch>::m_rx;

template<spi_channel ch>
uint8_t spi<ch>::m_status = spi<ch>::tx_complete | spi<ch>::rx_complete;

template<spi_channel ch>
safe_storage<typename spi<ch>::handler_fn> spi<ch>::m_handler_storage;

template<spi_channel ch>
ecl::err spi<ch>::init()
{
    if (m_status & inited) {
        return ecl::err::ok;
    }

    m_handler_storage.init();

    /* TODO: fill this when implementing real driver */

    m_status |= inited;

    return ecl::err::ok;
}

template<spi_channel ch>
void spi<ch>::set_rx(uint8_t *rx, size_t size)
{
    ecl_assert(m_status & inited);

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

template<spi_channel ch>
void spi<ch>::set_tx(size_t size, uint8_t fill_byte)
{
    ecl_assert(m_status & inited);

    m_status |= mode_fill;
    m_status &= ~(tx_hidden);
    m_tx.byte = fill_byte;
    m_tx_size = size;
}

template<spi_channel ch>
void spi<ch>::set_tx(const uint8_t *tx, size_t size)
{
    ecl_assert(m_status & inited);

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


template<spi_channel ch>
void spi<ch>::set_handler(const handler_fn &handler)
{
    get_handler() = handler;
}

template<spi_channel ch>
void spi<ch>::reset_buffers()
{
    m_status &= ~(mode_fill);
    m_tx.buf = nullptr;
    m_tx_size = 0;
    m_rx      = nullptr;
    m_rx_size = 0;
}

template<spi_channel ch>
void spi<ch>::reset_handler()
{
    get_handler() = handler_fn{};
}

template<spi_channel ch>
ecl::err spi<ch>::do_xfer()
{
    ecl_assert(m_status & inited);
    ecl_assert(valid_sizes());

    /* TODO: fill this when implementing real driver */

    return ecl::err::ok;
}

template<spi_channel ch>
void spi<ch>::enable_circular_mode()
{
    // Cannot enable circular mode if transfer is ongoing
    ecl_assert((m_status & tx_complete) && (m_status & rx_complete));

    m_status |= mode_circular;
}

template<spi_channel ch>
void spi<ch>::disable_circular_mode()
{
    m_status &= ~mode_circular;
}

template<spi_channel ch>
bool spi<ch>::is_circular_mode()
{
    return m_status & mode_circular;
}

//------------------------------------------------------------------------------

template<spi_channel ch>
bool spi<ch>::valid_sizes()
{
    // Bus is in full-duplex mode. Different sizes are not permitted.
    if (m_tx_size && m_rx_size) {
        if (m_tx_size != m_rx_size) {
            return false;
        }
    }

    return true;
}

template<spi_channel ch>
void spi<ch>::irq_handler()
{
    /* TODO: fill this when implementing real driver */
}

//! @}

//! @}

//! @}

} // namespace ecl

#endif // PLATFORM_SPI_BUS_HPP_
