//! \file
//! \brief Electron UART driver.
//! \details Based on Serial driver:
//! https://docs.particle.io/reference/firmware/electron/#serial
#ifndef PLATFORM_UART_BUS_HPP_
#define PLATFORM_UART_BUS_HPP_

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

//! Represents distinct peripheral devices
//! According to the particle electron wiring and docs, there are no
//! Serial3 device. See RM.
enum class uart_device
{
    serial0,
    serial1,
    serial2,
    serial4,
    serial5,
};

//------------------------------------------------------------------------------

void serial_tx_dispatch();

template<uart_device dev>
void serial_rx_dispatch();

//! \brief Electron UART bus wrapper
template<uart_device dev>
class uart_bus
{
    friend void serial_tx_dispatch();
    friend void serial_rx_dispatch<dev>();

public:
    // Convenient type aliases.
    using channel       = ecl::bus_channel;
    using event         = ecl::bus_event;
    using handler_fn    = ecl::bus_handler;

    // Static interface only
    uart_bus() = delete;
    ~uart_bus() = delete;

    // Should not be copied.
    uart_bus &operator=(uart_bus&) = delete;
    uart_bus(uart_bus&) = delete;

    //! \brief Lazy initialization.
    //! \return Status of operation.
    static err init();

    //! \brief Sets rx buffer with given size.
    //! \param[in,out]  rx      Buffer to write data to. Optional.
    //! \param[in]      size    Size
    static err set_rx(uint8_t *rx, size_t size);

    //! \brief Sets tx buffer made-up from sequence of similar bytes.
    //! \param[in] size         Size of sequence
    //! \param[in] fill_byte    Byte to fill a sequence. Optional.
    static err set_tx(size_t size, uint8_t fill_byte = 0xff);

    //! \brief Sets tx buffer with given size.
    //! \param[in] tx   Buffer to transmit. Optional.
    //! \param[in] size Buffer size.
    static err set_tx(const uint8_t *tx, size_t size);

    //! \brief Sets event handler.
    //! Handler will be used by the bus, until reset_handler() will be called.
    //! \param[in] handler Handler itself.
    static err set_handler(const handler_fn &handler);

    //! \brief Reset xfer buffers.
    //! Buffers that were set by \sa set_tx() and \sa set_rx()
    //! will be no longer used after this call.
    static err reset_buffers();

    //! \brief Resets previously set handler.
    static err reset_handler();

    //! \brief Executes xfer, using buffers previously set.
    //! When it will be done, handler will be invoked.
    //! \return Status of operation.
    static err do_xfer();

    static err do_tx();
    static err do_rx();

    //! \brief Cancels xfer.
    //! After this call no xfer will occur.
    //! \return Status of operation.
    static err cancel_xfer();

private:
    //! Handles events from Particle Serial drivers.
    //! \param[in] ev Serial channel that produced an event - RX or TX.
    static void serial_event(channel ch);

    //! Gets correct serial object for the device.
    template<uart_device d = dev, typename std::enable_if_t<d == uart_device::serial0, int> = 0>
    static constexpr USBSerial &get_serial();

    //! Gets correct serial object for the device.
    template<uart_device d = dev, typename std::enable_if_t<d != uart_device::serial0, int> = 0>
    static constexpr USARTSerial &get_serial();

    //! Set in m_status field if TX already done.
    //! \details While examining this bit, serial dispatcher decides whether
    //! event should be triggered or not.
    static constexpr uint8_t tx_done = 1;
    //! Set in m_status field if RX is pending.
    //! \details While examining this bit, serial dispatcher decides whether
    //! RX should be performed or not.
    static constexpr uint8_t rx_pend = 1 << 1;

    static safe_storage<handler_fn>     m_ev_handler;    //! Storage for event handler.

    static const uint8_t                *m_tx;           //! Transmit buffer.
    static size_t                       m_tx_size;       //! TX buffer size.
    static uint8_t                      *m_rx;           //! Receive buffer.
    static size_t                       m_rx_size;       //! RX buffer size.
    static size_t                       m_rx_next;       //! Next byte to fill in RX buffer.
    static std::atomic<uint8_t>         m_status;        //! Driver status
};

//------------------------------------------------------------------------------

template<uart_device dev>
safe_storage<typename uart_bus<dev>::handler_fn> uart_bus<dev>::m_ev_handler;

template<uart_device dev>
const uint8_t *uart_bus<dev>::m_tx;

template<uart_device dev>
uint8_t *uart_bus<dev>::m_rx;

template<uart_device dev>
size_t uart_bus<dev>::m_tx_size;

template<uart_device dev>
size_t uart_bus<dev>::m_rx_size;

template<uart_device dev>
size_t uart_bus<dev>::m_rx_next;

template<uart_device dev>
std::atomic<uint8_t> uart_bus<dev>::m_status;

//------------------------------------------------------------------------------

template<uart_device dev>
err uart_bus<dev>::init()
{
    // TODO: assert if not inited

    auto &serial = get_serial();

    serial.begin();
    m_ev_handler.init();

    return err::ok;
}

template<uart_device dev>
err uart_bus<dev>::set_rx(uint8_t *rx, size_t size)
{
    // TODO: assert if not inited

    m_rx = rx;
    m_rx_size = size;
    m_rx_next = 0;

    return err::ok;
}

template<uart_device dev>
err uart_bus<dev>::set_tx(size_t size, uint8_t fill_byte)
{
    (void)size;
    (void)fill_byte;
    // TODO
    return err::generic;
}

template<uart_device dev>
err uart_bus<dev>::set_tx(const uint8_t *tx, size_t size)
{
    // TODO: assert if not inited

    m_tx = tx;
    m_tx_size = size;

    return err::ok;
}

template<uart_device dev>
err uart_bus<dev>::set_handler(const handler_fn &handler)
{
    // TODO: assert if not inited
    m_ev_handler.get() = handler;
    return err::ok;
}

template<uart_device dev>
err uart_bus<dev>::reset_buffers()
{
    // TODO: assert if not inited

    m_status &= ~(tx_done | rx_pend);

    m_tx = nullptr;
    m_tx_size = 0;

    m_rx = nullptr;
    m_rx_size = 0;
    m_rx_next = 0;

    return err::ok;
}

template<uart_device dev>
err uart_bus<dev>::reset_handler()
{
    // TODO: assert if not inited

    m_ev_handler.get() = handler_fn{};
    return err::ok;
}

template<uart_device dev>
err uart_bus<dev>::do_tx()
{
    // TODO: assert if not inited

    auto &serial = get_serial();

    if (m_tx) {
        serial.write(m_tx, m_tx_size);
        m_status |= tx_done;
    }

    return err::ok;
}

template<uart_device dev>
err uart_bus<dev>::do_rx()
{
    // TODO: assert if not inited

    if (m_rx) {
        m_status |= rx_pend;
    }

    return err::ok;
}

template<uart_device dev>
err uart_bus<dev>::do_xfer()
{
    // TODO: assert if not inited

    auto rc = do_tx();
    if (is_error(rc)) {
        return rc;
    }

    return do_rx();
}

template<uart_device dev>
err uart_bus<dev>::cancel_xfer()
{
    // TODO
    return err::generic;
}

//------------------------------------------------------------------------------

template<uart_device dev>
void uart_bus<dev>::serial_event(channel ch)
{
    // TODO: assert if not inited

    auto &serial = get_serial();
    auto &handler = m_ev_handler.get();

    switch (ch) {
    case channel::tx:
        // Particle API allows to transfer bulk data and it is fine
        // at this moment. No intermediate events take place, only last one.
        // TX is completed.
        ecl_assert(m_status & tx_done);
        handler(channel::tx, event::tc, m_tx_size);
        m_status &= ~tx_done;
        break;
    case channel::rx:
        ecl_assert(m_status & rx_pend);
        while (serial.available()) {
            auto ch = serial.read();
            m_rx[m_rx_next++] = ch;

            // RX is completed
            if (m_rx_next >= m_rx_size) {
                handler(channel::rx, event::tc, m_rx_size);
                m_rx_next = 0;
                m_status &= ~rx_pend;
                break;
            }
        }
        break;
    default:
        break;
    }

    if (!(m_status & (rx_pend | tx_done))) {
        handler(channel::meta, event::tc, 0);
    }
}

template<uart_device dev>
template<uart_device d, typename std::enable_if_t<d == uart_device::serial0, int>>
constexpr USBSerial &uart_bus<dev>::get_serial()
{
    return Serial;
}

template<uart_device dev>
template<uart_device d, typename std::enable_if_t<d != uart_device::serial0, int>>
constexpr USARTSerial &uart_bus<dev>::get_serial()
{
    switch (dev) {
    case uart_device::serial1:
        return Serial1;
    case uart_device::serial2:
        return Serial2;
    case uart_device::serial4:
        return Serial4;
    case uart_device::serial5:
        return Serial5;
    }
}

} // namespace ecl

#endif // PLATFORM_UART_BUS_HPP_
