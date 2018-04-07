/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Electron I2C driver.
//! \details RM:
//! https://docs.particle.io/reference/firmware/electron/#wire-i2c-
#ifndef PLATFORM_I2C_BUS_HPP_
#define PLATFORM_I2C_BUS_HPP_

#include <common/bus.hpp>
#include <ecl/err.hpp>
#include <ecl/utils.hpp>
#include <ecl/assert.h>

#include <cstdint>
#include <unistd.h>

#include <functional>
#include <type_traits>
#include <atomic>

#include <application.h>

namespace ecl
{

//! List of I2C devices.
//! \details Only one device is present in particle electron board.
//! This enum is present to provide consistent interface accross
//! theCore platforms.
enum class i2c_device
{
    wire0
};

//! Base template class for the I2C configuration.
//! \details Configuration parameters for given I2C device are set
//! by creating a template specialization. The template specialization must
//! contain following fields to be recognized as valid:
//! - unsigned int speed  - I2C bus speed.
//! - bool stretch_clk    - If set to true, then clock stretching is enabled.
//! \par Configuration example.
//! \code{.cpp}
//! namespace ecl
//! {
//!
//! template<>
//! struct i2c_bus_cfg<i2c_device::wire0>
//! {
//!     static constexpr auto speed         = 10000; // 10 kHz
//!     static constexpr auto stretch_clk   = true;
//! };
//!
//! } // namespace ecl
//! \endcode
template<i2c_device dev>
struct i2c_bus_cfg
{
    // Always assert
    static_assert(std::is_integral<decltype(dev)>::value,
                  "The instance of this generic class should never be "
                  "instantiated. Please write your own template specialization "
                  "of this class. See documentation.");
};

//! I2C bus based on Particle electron drivers.
//! \tparam Speed       I2C bus speed in HZ.
//! \tparam StretchClk  Enable or disable clock stretching.
template<i2c_device dev>
class i2c_bus
{
    template<class I2C_Dev>
    friend void dispatch();
public:
    //! Lazy initialization.
    //! \return Status of operation.
    static err init();

    //! Sets rx buffer with given size.
    //! \param[in,out]  rx      Buffer to write data to. Optional.
    //! \param[in]      size    Size
    static void set_rx(uint8_t *rx, size_t size);

    //! Sets rx buffer made-up from sequence of similar bytes.
    //! \param[in] size         Size of sequence
    //! \param[in] fill_byte    Byte to fill a sequence. Optional.
    static void set_tx(size_t size, uint8_t fill_byte = 0xff);

    //! Sets tx buffer with given size.
    //! \param[in] tx   Buffer to transmit. Optional.
    //! \param[in] size Buffer size.
    static void set_tx(const uint8_t *tx, size_t size);

    //! Sets event handler.
    //! Handler will be used by the bus, until reset_handler() will be called.
    //! \param[in] handler Handler itself.
    static void set_handler(const bus_handler &handler);

    //! Reset xfer buffers.
    //! Buffers that were set by \sa set_tx() and \sa set_rx()
    //! will be no longer used after this call.
    static void reset_buffers();

    //! Resets previously set handler.
    static void reset_handler();

    //! Executes xfer, using buffers previously set.
    //! \details When it will be done, handler will be invoked.
    //! \return Status of operation.
    static err do_xfer();

    //! Cancels xfer.
    //! After this call no xfer will occur.
    //! \return Status of operation.
    static err cancel_xfer();

    //! Sets slave address.
    //! \details This address is used during communication. In case if
    //! there is only one device on the bus, this function may be called
    //! only once, before first xfer().
    static void set_slave_addr(uint16_t addr);

private:
    //! Handles events from event dispatcher.
    static void dispatcher_event_handler(bus_channel ch, bus_event ev);

    //! If set, then TX xfer is pending
    static constexpr auto tx_pend = 1;

    //! If set, then RX xfer is requested.
    static constexpr auto rx_req = 1 << 1;

    static safe_storage<bus_handler>     m_ev_handler; //! Storage for event handler.
    static const uint8_t                 *m_tx;        //! TX buffer.
    static size_t                        m_tx_size;    //! TX buffer size.
    static uint8_t                       *m_rx;        //! RX buffer.
    static size_t                        m_rx_size;    //! RX buffer size.
    static uint16_t                      m_addr;       //! I2C target slave address.
    static uint8_t                       m_status;     //! Bus status.
};

template<i2c_device dev> safe_storage<bus_handler> i2c_bus<dev>::m_ev_handler{};
template<i2c_device dev> const uint8_t             *i2c_bus<dev>::m_tx{};
template<i2c_device dev> size_t                    i2c_bus<dev>::m_tx_size{};
template<i2c_device dev> uint8_t                   *i2c_bus<dev>::m_rx{};
template<i2c_device dev> size_t                    i2c_bus<dev>::m_rx_size{};
template<i2c_device dev> uint16_t                  i2c_bus<dev>::m_addr{};
template<i2c_device dev> uint8_t                   i2c_bus<dev>  ::m_status{};

//------------------------------------------------------------------------------

template<i2c_device dev>
err i2c_bus<dev>::init()
{
    // TODO: assert if already inited

    m_ev_handler.init();

    Wire.setSpeed(i2c_bus_cfg<dev>::speed);
    Wire.stretchClock(i2c_bus_cfg<dev>::stretch_clk);
    Wire.begin();

    return err::ok;
}

template<i2c_device dev>
void i2c_bus<dev>::set_rx(uint8_t *rx, size_t size)
{
    m_rx = rx;
    m_rx_size = size;
}

template<i2c_device dev>
void i2c_bus<dev>::set_tx(const uint8_t *tx, size_t size)
{
    m_tx = tx;
    m_tx_size = size;
}

template<i2c_device dev>
void i2c_bus<dev>::set_handler(const bus_handler &handler)
{
    m_ev_handler.get() = handler;
}

template<i2c_device dev>
void i2c_bus<dev>::reset_buffers()
{
    m_tx = m_rx = nullptr;
    m_rx_size = m_tx_size = 0;
    m_status &= ~(tx_pend | rx_req);
}

template<i2c_device dev>
void i2c_bus<dev>::reset_handler()
{
    m_ev_handler.get() = bus_handler{};
}

template<i2c_device dev>
err i2c_bus<dev>::do_xfer()
{
    if (m_tx) {
        m_status |= tx_pend;
    }

    if (m_rx) {
        m_status |= rx_req;
    }

    // Launch xfer and soft-IRQ
    extern void i2c_dispatch();
    i2c_dispatch();

    return err::ok;
}

template<i2c_device dev>
err i2c_bus<dev>::cancel_xfer()
{
    // TODO
    ecl_assert_msg(0, "Not implemented!");
    return err::generic;
}

template<i2c_device dev>
void i2c_bus<dev>::set_slave_addr(uint16_t addr)
{
    // Incoming addresses represent full-byte address, with R/W-marker LSB
    // included. But wire particle driver expects only 7 MSB without R/W marker.
    // That's why address is shifted to remove that LSB.
    // Set I2C addressing protocol for details.
    m_addr = addr >> 1;
}

//------------------------------------------------------------------------------

template<i2c_device dev>
void i2c_bus<dev>::dispatcher_event_handler(bus_channel ch, bus_event ev)
{
    size_t sz = 0;
    auto &evh = m_ev_handler.get();

    if (ev == bus_event::tc) {
        switch (ch) {
            case bus_channel::tx:
                sz = m_tx_size;
                m_status &= ~tx_pend;
                break;
            case bus_channel::rx:
                sz = m_rx_size;
                m_status &= ~rx_req;
                break;
            default:
                // TODO: raise error?
                break;
        }
    }

    evh(ch, ev, sz);

    if (!(m_status & (rx_req | tx_pend))) {
        evh(bus_channel::meta, bus_event::tc, 0);
    }
}

} // namespace ecl

#endif // PLATFORM_I2C_BUS_HPP_
