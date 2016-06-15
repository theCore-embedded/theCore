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
    ecl::err init()
    { return err::ok; }

    //!
    //! \brief Sets rx buffer with given size.
    //! \param[in,out]  rx      Buffer to write data to. Optional.
    //! \param[in]      size    Size
    //!
    void set_rx(uint8_t *rx, size_t size)
    {
        m_rx = rx;
        m_rx_size = size;
    }

    //!
    //! \brief Sets tx buffer with given size.
    //! \param[in] tx   Buffer to transmit. Optional.
    //! \param[in] size Buffer size.
    //!
    void set_tx(const uint8_t *tx, size_t size)
    {
        m_tx = tx;
        m_tx_size = size;
    }

    //!
    //! \brief Sets event handler.
    //! Handler will be used by the bus, until reset_handler() will be called.
    //! \param[in] handler Handler itself.
    //!
    void set_handler(const handler_fn &handler)
    {
        m_fn = handler;
    }

    //!
    //! \brief Reset xfer buffers.
    //! Buffers that were set by \sa set_tx() and \sa set_rx()
    //! will be no longer used after this call.
    //!
    void reset_buffers()
    {
        m_tx = m_rx = nullptr;
    }

    //!
    //! \brief Resets previously set handler.
    //!
    void reset_handler()
    {
        m_fn = {};
    }

    //!
    //! \brief Executes xfer, using buffers previously set.
    //! When it will be done, handler will be invoked.
    //! \return Status of operation.
    //!
    ecl::err do_xfer()
    {
        if (m_tx && m_tx_size) {
            auto tx = m_tx;
            auto size = m_tx_size;
            while (size--) { std::putchar(*tx++); }
            m_fn(channel::tx, event::tc, m_tx_size);
        }

        if (m_rx && m_rx_size) {
            auto rx = m_rx;
            auto size = m_rx_size;
            while(size--) { *rx++ = std::getchar(); }
            m_fn(channel::rx, event::tc, m_rx_size);
        }

        m_fn(channel::meta, event::tc, 0);
        return err::ok;
    }

private:
    uint8_t           *m_rx;
    const uint8_t     *m_tx;
    size_t            m_tx_size;
    size_t            m_rx_size;
    handler_fn        m_fn;
};

//! Bypasses console drivers and puts data directly to the UART
//! \details Required to print debug of the failed asserts. Check other platforms
//! to get idea why this function is provided.
static inline void bypass_putc(char c)
{
    putchar(c);
}

}

#endif // HOST_PLATFORM_CONSOLE_HPP_
