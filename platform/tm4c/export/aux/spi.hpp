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

#include <driverlib/ssi.h>

namespace ecl
{

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup tm4c Texas Instruments Tiva C TM4C123G platform
//! @{

//! \defgroup tm4c_spi SPI driver
//! @{

//! List of avaliable SPI channels.
enum class spi_channel
{
    ch0 = SSI0_BASE,
    ch1 = SSI1_BASE,
    ch2 = SSI2_BASE,
    ch3 = SSI3_BASE,
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
//!  - SPI sysclock divider. Must be greather than 2
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
//!     static constexpr auto type      = spi_type::master;
//!     static constexpr auto cpol      = spi_cpol::high;
//!     static constexpr auto cpha      = spi_cpha::low;
//!     static constexpr auto clk_div   = 2;
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

    //! \brief Cancels xfer.
    //! After this call no xfer will occur.
    //! \return Status of operation.
    static ecl::err cancel_xfer();

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
    //! Gets SYSCTL associated with given SPI.
    static constexpr auto pick_sysctl();
    //! Gets SPI interrupt number.
    static constexpr auto pick_it();
    //! Gets SPI mode (protocol).
    static constexpr auto pick_protocol();

    //! Bus is inited if this flag is set.
    static constexpr uint8_t inited        = 1;
    //! Bus is in fill mode if this flag is set.
    static constexpr uint8_t mode_fill     = 1 << 1;
    //! Bus is in circular mode if this flag is set.
    static constexpr uint8_t mode_circular = 1 << 2;
    //! TX is finished if this flag is set.
    static constexpr uint8_t tx_complete   = 1 << 3;
    //! User will not be notified about TX events if this flag is set.
    //! \details This flag is required to prevent event propagation in
    //! case if TX stream must be ignored.
    static constexpr uint8_t tx_hidden     = 1 << 4;
    //! RX is finished if this flag is set.
    static constexpr uint8_t rx_complete   = 1 << 5;
    //! User will not be notified about RX events if this flag is set.
    //! \details This flag is required to prevent event propagation in
    //! case if RX stream must be ignored.
    static constexpr uint8_t rx_hidden     = 1 << 6;
    //! Bit set if xfer was canceled.
    static constexpr uint8_t xfer_canceled = 1 << 7;

    static union tx_data
    {
        const uint8_t *buf;         //! Transmit buffer.
        uint16_t      byte;         //! Byte to transmit (16 bits of 16-bit SPI mode)
    } m_tx;

    static size_t  m_tx_size;       //! TX buffer size.
    static size_t  m_tx_idx;        //! TX buffer index.
    static uint8_t *m_rx;           //! Receive buffer.
    static size_t  m_rx_size;       //! RX buffer size.
    static size_t  m_rx_idx;        //! RX buffer index.
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
size_t spi<ch>::m_tx_idx;

template<spi_channel ch>
size_t spi<ch>::m_rx_idx;

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
    ecl_assert(!(m_status & inited));
    /* Only master mode is supported for now */
    static_assert(config::type == spi_type::master, "modes other than master is not supported!");

    constexpr auto periph = extract_value(ch);
    constexpr auto periph_sysctl = pick_sysctl();
    constexpr auto protocol = pick_protocol();
    constexpr auto clk_div = config::clk_div;

    m_handler_storage.init();

    SysCtlPeripheralEnable(periph_sysctl);

    // TODO:
    // Ensure that the SSE bit in the SSICR1 register is clear
    // before making any configuration changes.
    // See datasheet, section 15.4. Initialization and Configuration

    // According to the driver lib documentation (driverlib/ssi.c):
    // The ui32BitRate parameter defines the bit rate for the SSI.  This bit
    // rate must satisfy the following clock ratio criteria:
    //
    // - FSSI >= 2 * bit rate (master mode)
    // - FSSI >= 12 * bit rate (slave modes)
    //
    // where FSSI is the frequency of the clock supplied to the SSI module. Note
    // that there are frequency limits for FSSI that are described in the Bit Rate
    // Generation section of the SSI chapter in the data sheet.

    static_assert(clk_div >= 2, "clock divider for SSI (SPI) must be greather than 2");

    SSIConfigSetExpClk(periph, SysCtlClockGet(), protocol, SSI_MODE_MASTER,
             SysCtlClockGet() / clk_div,    // Specifies SPI clock. See comment above.
             8);                            // 8 bit data transfer

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
        // RX channel will be enabled in bidir SPI even if user don't
        // want it. SPI IRQ handler must not propagate RX events in this case.
        m_status |= rx_hidden;
        return;
    }

    m_rx      = rx;
    m_rx_size = size;
    m_status &= ~(rx_hidden);
    m_rx_idx  = 0;
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
    m_tx_idx  = 0;
}


template<spi_channel ch>
void spi<ch>::set_handler(const handler_fn &handler)
{
    get_handler() = handler;
}

template<spi_channel ch>
void spi<ch>::reset_buffers()
{
    m_status  &= ~(mode_fill);
    m_tx.buf  = nullptr;
    m_tx_size = 0;
    m_tx_idx  = 0;
    m_rx      = nullptr;
    m_rx_size = 0;
    m_rx_idx  = 0;
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

    constexpr auto periph = extract_value(ch);
    constexpr auto irqn = pick_it();

    uint32_t irq_flags = 0;

    if (m_rx) {
        // To be able to receive data, clock must be supplied by writing data
        // into the TX stream.
        irq_flags |= (SSI_RXOR | SSI_TXFF); // TODO: handle rx timeout interrupt?
    }

    if (m_tx.buf || (m_status & rx_hidden) || (m_status & mode_fill)) {
        irq_flags |= SSI_TXFF;
    }

    m_tx_idx = m_rx_idx = 0;

    SSIEnable(periph);
    SSIIntEnable(periph, irq_flags);

    // Transaction will me managed in the interrupt handler
    irq::unmask(irqn);

    return ecl::err::ok;
}

template<spi_channel ch>
ecl::err spi<ch>::cancel_xfer()
{
    ecl_assert(m_status & inited);
    m_status |= xfer_canceled;

    // TODO: Implement cancellation

    return ecl::err::notsup;
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
constexpr auto spi<ch>::pick_it()
{
    switch (ch) {
        case spi_channel::ch0:
            return INT_SSI0;
        case spi_channel::ch1:
            return INT_SSI1;
        case spi_channel::ch2:
            return INT_SSI2;
        case spi_channel::ch3:
            return INT_SSI3;
    }
}

template<spi_channel ch>
constexpr auto spi<ch>::pick_sysctl()
{
    switch (ch) {
        case spi_channel::ch0:
            return SYSCTL_PERIPH_SSI0;
        case spi_channel::ch1:
            return SYSCTL_PERIPH_SSI1;
        case spi_channel::ch2:
            return SYSCTL_PERIPH_SSI2;
        case spi_channel::ch3:
            return SYSCTL_PERIPH_SSI3;
    }
}

template<spi_channel ch>
void spi<ch>::irq_handler()
{
    constexpr auto periph = extract_value(ch);
    constexpr auto irqn = pick_it();

    // TODO: test interrupt flags instead of status register

    constexpr auto ssisr_tnf_bit  = (1 << 1); // SSI Transmit FIFO Not Full
    constexpr auto ssisr_rne_bit  = (1 << 2); // SSI Receive FIFO Not Empty
    constexpr auto ssisr_reg_offt = 0xc;      // SSI SR offset

    const auto &handler = get_handler();
    auto irq_status = SSIIntStatus(periph, true);
    auto status = HWREG(periph + ssisr_reg_offt);

    // Treat hidden RX/TX as always completed

    bool rx_done = (m_status & rx_hidden);
    bool tx_done = (m_status & tx_hidden);

    // In case of hardware faulty, IRQ handler *can* be potentially
    // invoked. In that case, user-level handler may be invalid.

    if (!handler) {
        ecl::abort();
    }

    // Since SPI operating in full-duplex mode, TXFF interrupt allows
    // to process both RX and TX

    if ((irq_status & SSI_RXOR) && m_rx_idx < m_rx_size) {
        handler(bus_channel::rx, bus_event::err, m_rx_idx);
        SSIIntClear(periph, SSI_RXOR);
        rx_done = tx_done = true; // Do not process anything. Stop here.
    } else {
        if (status & ssisr_tnf_bit) {
            if (m_tx_idx < m_tx_size) {
                uint8_t tx_byte = m_status & mode_fill
                        ? m_tx.byte : m_tx.buf[m_tx_idx];

                m_tx_idx++;
                SSIDataPut(periph, tx_byte);

                if (m_tx_idx == (m_tx_size >> 1)) {
                    handler(channel::tx, event::ht, m_tx_idx);
                } else if (m_tx_idx == m_tx_size) {
                    handler(channel::tx, event::tc, m_tx_idx);
                }
            } else if ((m_status & tx_hidden) && m_rx_idx < m_rx_size) {
                SSIDataPut(periph, 0xff); // Dummy byte
            } else {  // One event ago TX was completed.
                tx_done = true;
            }
        }

        if (status & ssisr_rne_bit) {
            if (m_rx_idx < m_rx_size) {
                ecl_assert(m_rx);

                uint32_t data;
                SSIDataGet(periph, &data);
                m_rx[m_rx_idx++] = (uint8_t)data;

                if (m_rx_idx == (m_rx_size >> 1)) {
                    handler(channel::rx, event::ht, m_rx_idx);
                } else if (m_rx_idx == m_rx_size) {
                    handler(channel::rx, event::tc, m_rx_idx);
                }
            } else if (m_status & rx_hidden) {
                // Ignore RX data
                uint32_t data;
                SSIDataGet(periph, &data);
            } else { // One event ago RX was completed.
                rx_done = true;
            }
        }
    }

    irq::clear(irqn);

    // TODO: handle cancel xfer

    if (rx_done && tx_done) {
        handler(bus_channel::meta, bus_event::tc, m_tx_size ? m_tx_size : m_rx_size);

        // TODO: restart or abort transaction in case of error?
        if (m_status & mode_circular) {
            m_tx_idx = m_rx_idx = 0;
        } else {
            SSIIntDisable(periph, SSI_RXTO | SSI_RXOR | SSI_TXFF);
            SSIDisable(periph);
            irq::mask(irqn);
            return;
        }
    }

    irq::unmask(irqn);
}

template<spi_channel ch>
constexpr auto spi<ch>::pick_protocol()
{
    if (config::cpol == spi_cpol::low) {
        if (config::cpha == spi_cpha::low) {
            return SSI_FRF_MOTO_MODE_0;
        } else {
            return SSI_FRF_MOTO_MODE_1;
        }
    } else {
        if (config::cpha == spi_cpha::low) {
            return SSI_FRF_MOTO_MODE_2;
        } else {
            return SSI_FRF_MOTO_MODE_3;
        }
    }
}

//! @}

//! @}

//! @}

} // namespace ecl

#endif // PLATFORM_SPI_BUS_HPP_
