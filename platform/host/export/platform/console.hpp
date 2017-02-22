#ifndef HOST_PLATFORM_CONSOLE_HPP_
#define HOST_PLATFORM_CONSOLE_HPP_

#include <iostream>
#include <functional>

#include <common/bus.hpp>
#include <ecl/err.hpp>
#include <stdio.h>

namespace ecl
{

class platform_console
{
public:
    // Convenient type aliases.
    using channel       = ecl::bus_channel;
    using event         = ecl::bus_event;
    using handler_fn    = ecl::bus_handler;

    //!
    //! \brief Lazy initialization.
    //! \return Status of operation.
    //!
    static ecl::err init()
    {
        new (&m_fn_storage) handler_fn;
        return err::ok;
    }

    //!
    //! \brief Sets rx buffer with given size.
    //! \param[in,out]  rx      Buffer to write data to. Optional.
    //! \param[in]      size    Size
    //!
    static void set_rx(uint8_t *rx, size_t size)
    {
        m_rx = rx;
        m_rx_size = size;
    }

    //!
    //! \brief Sets tx buffer with given size.
    //! \param[in] tx   Buffer to transmit. Optional.
    //! \param[in] size Buffer size.
    //!
    static void set_tx(const uint8_t *tx, size_t size)
    {
        m_tx = tx;
        m_tx_size = size;
    }

    //!
    //! \brief Sets event handler.
    //! Handler will be used by the bus, until reset_handler() will be called.
    //! \param[in] handler Handler itself.
    //!
    static void set_handler(const handler_fn &handler)
    {
        get_fn() = handler;
    }

    //!
    //! \brief Reset xfer buffers.
    //! Buffers that were set by \sa set_tx() and \sa set_rx()
    //! will be no longer used after this call.
    //!
    static void reset_buffers()
    {
        m_tx = m_rx = nullptr;
    }

    //!
    //! \brief Resets previously set handler.
    //!
    static void reset_handler()
    {
        get_fn() = {};
    }

    //!
    //! \brief Executes xfer, using buffers previously set.
    //! When it will be done, handler will be invoked.
    //! \return Status of operation.
    //!
    static ecl::err do_xfer()
    {
        if (m_tx && m_tx_size) {
            auto tx = m_tx;
            auto size = m_tx_size;
            while (size--) { std::putchar(*tx++); }
            get_fn()(channel::tx, event::tc, m_tx_size);
        }

        if (m_rx && m_rx_size) {
            auto rx = m_rx;
            auto size = m_rx_size;
            while(size--) { *rx++ = std::getchar(); }
            get_fn()(channel::rx, event::tc, m_rx_size);
        }

        get_fn()(channel::meta, event::tc, 0);
        return err::ok;
    }

    //!
    //! \brief Cancels xfer.
    //! After this call no xfer will occur.
    //! \return Status of operation.
    //!
    static err cancel_xfer()
    { return err::nosys; }

private:
    static uint8_t           *m_rx;
    static const uint8_t     *m_tx;
    static size_t            m_tx_size;
    static size_t            m_rx_size;

    //! Static handler storage
    static std::aligned_storage_t<sizeof(handler_fn), alignof(handler_fn)> m_fn_storage;

    //! Gets handler from storage
    static constexpr handler_fn &get_fn() { return reinterpret_cast<handler_fn&>(m_fn_storage); }

};

//! Bypasses console drivers and puts data directly to the UART
//! \details Required to print debug of the failed asserts. Check other platforms
//! to get idea why this function is provided.
static inline void bypass_putc(char c)
{
    putchar(c);
}

} // namespace ecl

#endif // HOST_PLATFORM_CONSOLE_HPP_
