//! \file
//! \brief Electron UART driver
#ifndef PLATFORM_UART_BUS_HPP_
#define PLATFORM_UART_BUS_HPP_

#include <common/bus.hpp>
#include <ecl/err.hpp>
#include <ecl/utils.hpp>

#include <cstdint>
#include <unistd.h>

#include <functional>
#include <type_traits>

#include <application.h>

namespace ecl
{

//! Represents distinct peripheral devices
enum class uart_device
{
    serial0,
    serial1,
    serial2,
    serial4,
    serial5,
};

//! \brief Electron UART bus wrapper
template<uart_device dev>
class uart_bus
{
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

    //! \brief Cancels xfer.
    //! After this call no xfer will occur.
    //! \return Status of operation.
    static err cancel_xfer();

private:
    //! Gets correct serial object for the device.

    template<uart_device d = dev, typename std::enable_if_t<d == uart_device::serial0, int> = 0>
    static constexpr USBSerial &get_serial();

    //! Gets correct serial object for the device.
    template<uart_device d = dev, typename std::enable_if_t<d != uart_device::serial0, int> = 0>
    static constexpr USARTSerial &get_serial();

    static const uint8_t                *m_tx;           //! Transmit buffer.
    static size_t                       m_tx_size;       //! TX buffer size.
    static uint8_t                      *m_rx;           //! Receive buffer.
    static size_t                       m_rx_size;       //! RX buffer size.

    static safe_storage<handler_fn>     m_ev_storage;    //! Storage for event handler.
};

//------------------------------------------------------------------------------

template<uart_device dev>
safe_storage<typename uart_bus<dev>::handler_fn> uart_bus<dev>::m_ev_storage;

template<uart_device dev>
const uint8_t *uart_bus<dev>::m_tx;

template<uart_device dev>
uint8_t *uart_bus<dev>::m_rx;

template<uart_device dev>
size_t uart_bus<dev>::m_tx_size;

template<uart_device dev>
size_t uart_bus<dev>::m_rx_size;

//------------------------------------------------------------------------------

template<uart_device dev>
err uart_bus<dev>::init()
{
    auto &serial = get_serial();

    serial.begin();
    m_ev_storage.init();

    return err::ok;
}

template<uart_device dev>
err uart_bus<dev>::set_rx(uint8_t *rx, size_t size)
{
    m_rx = rx;
    m_rx_size = size;

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
    m_tx = tx;
    m_tx_size = size;

    return err::ok;
}

template<uart_device dev>
err uart_bus<dev>::set_handler(const handler_fn &handler)
{
    m_ev_storage.get() = handler;
    return err::ok;
}

template<uart_device dev>
err uart_bus<dev>::reset_buffers()
{
    m_tx = nullptr;
    m_tx_size = 0;

    m_rx = nullptr;
    m_rx_size = 0;

    return err::ok;
}

template<uart_device dev>
err uart_bus<dev>::reset_handler()
{
    m_ev_storage.get() = handler_fn{};
    return err::ok;
}

template<uart_device dev>
err uart_bus<dev>::do_xfer()
{
    auto &serial = get_serial();

    if (m_tx) {
        serial.write(m_tx, m_tx_size);
        m_ev_storage.get()(channel::tx, event::tc, m_tx_size);
    }

    m_ev_storage.get()(channel::meta, event::tc, 0);

    // TODO: RX.

    return err::ok;
}

template<uart_device dev>
err uart_bus<dev>::cancel_xfer()
{
    // TODO
    return err::generic;
}

//------------------------------------------------------------------------------

template<uart_device dev>
template<uart_device d, typename std::enable_if_t<d == uart_device::serial0, int>>
constexpr USBSerial &uart_bus<dev>::get_serial()
{
    switch (dev) {
    case uart_device::serial0:
        return Serial;
    }
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
