/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief UART implementation for Freescale Kinetis KE02 MCU
//! \ingroup ke02_uart

#ifndef PLATFORM_KE02_UART_HPP_
#define PLATFORM_KE02_UART_HPP_

#include <type_traits>

#include <common/bus.hpp>
#include <ecl/assert.h>
#include <ecl/utils.hpp>

// Platform vendor lib.
#include <common.h> // Test if we need this or not.
#include <uart.h>

namespace ecl
{

//! \addtogroup platform Platform defintions and drivers
//! @{

//! \addtogroup ke02 Freescale Kinetis KE02 platform
//! @{

//! \defgroup ke02_uart UART driver
//! @{

//! Represents distinct UART peripheral devices (channels)
enum class uart_channel
{
    ch0 = UART0_BASE,
    ch1 = UART1_BASE,
    ch2 = UART2_BASE,
};

//! UART driver.
//! \tparam ch Peripheral channel to use with this driver
template<uart_channel ch>
class uart
{
    //! Bypass console routines, partially reuse UART driver code.
    friend void bypass_console_init();
    friend void bypass_putc(char c);
public:

    //! \brief Lazy initialization.
    //! \return Status of operation.
    static err init();

    //! \brief Sets rx buffer with given size.
    //! \param[in,out]  rx      Buffer to write data to. Optional.
    //! \param[in]      size    Size
    static void set_rx(uint8_t *rx, size_t size);

    //! \brief Sets rx buffer made-up from sequence of similar bytes.
    //! \param[in] size         Size of sequence
    //! \param[in] fill_byte    Byte to fill a sequence. Optional.
    static void set_tx(size_t size, uint8_t fill_byte = 0xff);

    //! \brief Sets tx buffer with given size.
    //! \param[in] tx   Buffer to transmit. Optional.
    //! \param[in] size Buffer size.
    static void set_tx(const uint8_t *tx, size_t size);

    //! \brief Sets event handler.
    //! Handler will be used by the bus, until reset_handler() will be called.
    //! \param[in] handler Handler itself.
    static void set_handler(const bus_handler &handler);

    //! \brief Reset xfer buffers.
    //! Buffers that were set by \sa set_tx() and \sa set_rx()
    //! will be no longer used after this call.
    static void reset_buffers();

    //! \brief Resets previously set handler.
    static void reset_handler();

    //! \brief Executes xfer, using buffers previously set.
    //! When it will be done, handler will be invoked.
    //! \return Status of operation.
    static err do_xfer();

    //! \brief Cancels xfer.
    //! After this call no xfer will occur.
    //! \return Status of operation.
    static err cancel_xfer();

    uart(const uart&) = delete;
    uart &operator=(uart&) = delete;

private:
    //! Gets SYSCTL associated with given UART.
    static constexpr auto pick_sysctl();

    //! Gets UART bus context.
    static constexpr auto &get_ctx();

    //! Stub handler, called in case if no one listens for bus events.
    static void stub_handler(bus_channel, bus_event, size_t)
    {
        ecl_assert_msg(false, "UART event wasn't handled");
    }

    //! Private context of the UART bus driver.
    struct ctx
    {
        //! Constructs default context.
        ctx()
            :h{stub_handler},
            tx{nullptr},
            tx_sz{0},
            tx_idx{0},
            rx{nullptr},
            rx_sz{0},
            rx_idx{0},
            status{0}
        { }

        // Ctx is not  CopyConstructible.
        ctx(const ctx&) = delete;
        ctx &operator=(ctx&) = delete;

        //! Bit set in status field if bus is initialized.
        static constexpr uint8_t inited     = 0x1;
        //! Bit set in status field if bus is in fill mode.
        static constexpr uint8_t fill       = 0x2;
        //! Bit set in status field if bus is finished with TX.
        static constexpr uint8_t tx_done    = 0x4;
        //! Bit set in status field if bus is finished with RX.
        static constexpr uint8_t rx_done    = 0x8;

        bus_handler     h;          //! Event handler.
        union
        {
            const uint8_t   *buf;        //! TX buffer.
            uint8_t         fill_byte;   //! Fill byte.
        } tx;

        size_t          tx_sz;      //! TX buffer size.
        //! TX buffer current index.
        //! In fill mode it counts bytes written.
        size_t          tx_idx;
        uint8_t         *rx;        //! RX buffer.
        size_t          rx_sz;      //! RX buffer size.
        size_t          rx_idx;     //! RX buffer current index.
        uint8_t         status;     //! Bus status.
    };

    //! Private context storage.
    //! \details Default-initialized during static objects zero-initialization.
    //! Afterwards, init() is responsible of calling non-trival ctor of ctx.
    static safe_storage<ctx> m_ctx_storage;
};

template<uart_channel ch>
safe_storage<typename uart<ch>::ctx> uart<ch>::m_ctx_storage;

//------------------------------------------------------------------------------
// Private members

template<uart_channel ch>
constexpr auto uart<ch>::pick_sysctl()
{
    switch (ch) {
        case uart_channel::ch0:
            return UART0;
        case uart_channel::ch1:
            return UART1;
        case uart_channel::ch2:
            return UART2;
    }
}

template<uart_channel ch>
constexpr auto &uart<ch>::get_ctx()
{
    return m_ctx_storage.get();
}

//------------------------------------------------------------------------------

template<uart_channel ch>
err uart<ch>::init()
{
    // POD-type fields of ctx will be initialized to 0, so it is valid to access
    // flags there.
    auto &bus_ctx = get_ctx();

    ecl_assert(!bus_ctx.status);

    constexpr auto periph_sysctl = pick_sysctl();

    m_ctx_storage.init();

    UART_ConfigType sConfig;

    sConfig.u32SysClkHz = BUS_CLK_HZ;
    sConfig.u32Baudrate  = UART_PRINT_BITRATE; // 115200

    UART_Init(periph_sysctl, &sConfig);

    bus_ctx.status |= (ctx::inited | ctx::rx_done | ctx::tx_done);
    return err::ok;
}

template<uart_channel ch>
void uart<ch>::set_rx(uint8_t *rx, size_t size)
{
    auto &bus_ctx = get_ctx();

    ecl_assert((bus_ctx.status & (ctx::inited | ctx::rx_done))
               == (ctx::inited | ctx::rx_done));

    bus_ctx.rx = rx;
    bus_ctx.rx_sz = size;
    bus_ctx.rx_idx = 0;
}

template<uart_channel ch>
void uart<ch>::set_tx(const uint8_t *tx, size_t size)
{
    auto &bus_ctx = get_ctx();

    ecl_assert((bus_ctx.status & (ctx::inited | ctx::tx_done))
               == (ctx::inited | ctx::tx_done));

    bus_ctx.tx.buf = tx;
    bus_ctx.tx_sz = size;
    bus_ctx.tx_idx = 0;

    bus_ctx.status &= ~ctx::fill;
}

template<uart_channel ch>
void uart<ch>::set_tx(size_t size, uint8_t fill_byte)
{
    auto &bus_ctx = get_ctx();

    ecl_assert((bus_ctx.status & (ctx::inited | ctx::tx_done))
               == (ctx::inited | ctx::tx_done));

    bus_ctx.tx.fill_byte = fill_byte;
    bus_ctx.tx_sz = size;

    bus_ctx.status |= ctx::fill;
}

template<uart_channel ch>
void uart<ch>::set_handler(const bus_handler &handler)
{
    auto &bus_ctx = get_ctx();

    ecl_assert((bus_ctx.status & (ctx::inited | ctx::tx_done | ctx::rx_done))
               == (ctx::inited | ctx::tx_done | ctx::rx_done));

    bus_ctx.h = handler;
}

template<uart_channel ch>
void uart<ch>::reset_buffers()
{
    auto &bus_ctx = get_ctx();

    ecl_assert((bus_ctx.status & (ctx::inited | ctx::tx_done | ctx::rx_done))
               == (ctx::inited | ctx::tx_done | ctx::rx_done));

    // If buffers are not set then fill mode must be disabled to avoid
    // ambiguity.
    bus_ctx.status &= ~ctx::fill;
    bus_ctx.tx.buf = nullptr;
    bus_ctx.rx = nullptr;
}

template<uart_channel ch>
void uart<ch>::reset_handler()
{
    auto &bus_ctx = get_ctx();

    ecl_assert((bus_ctx.status & (ctx::inited | ctx::tx_done | ctx::rx_done))
               == (ctx::inited | ctx::tx_done | ctx::rx_done));

    bus_ctx.h = stub_handler;
}

template<uart_channel ch>
err uart<ch>::do_xfer()
{
    auto &bus_ctx = get_ctx();

    ecl_assert((bus_ctx.status & (ctx::inited | ctx::tx_done | ctx::rx_done))
               == (ctx::inited | ctx::tx_done | ctx::rx_done));

    // At least one of direction should be enabled
    ecl_assert(bus_ctx.tx.buf || bus_ctx.rx);

    if ((bus_ctx.status & ctx::fill) || bus_ctx.tx.buf) {
        UART_Type * ut = pick_sysctl();

        if (bus_ctx.status & ctx::fill) {
            for(size_t i = 0; i < bus_ctx.tx_sz; i++) {
                UART_PutChar(ut, bus_ctx.tx.fill_byte);
            }
        } else {
            while(bus_ctx.tx_idx < bus_ctx.tx_sz) {
                UART_PutChar(ut, bus_ctx.tx.buf[bus_ctx.tx_idx]);
                bus_ctx.tx_idx++;
            }   
        }

        bus_ctx.status |= ctx::tx_done;
    }

    if (bus_ctx.rx) {
        // TODO: Implement RX mechanism.
        bus_ctx.status |= ctx::rx_done;
    }

    bus_ctx.h(bus_channel::meta, bus_event::tc, 0);

    return err::ok;
}

template<uart_channel ch>
err uart<ch>::cancel_xfer()
{
    auto &bus_ctx = get_ctx();

    bus_ctx.status |= (ctx::tx_done & ctx::rx_done);

    return err::ok;
}

//! @}

//! @}

//! @}

} // namespace ecl

#endif // PLATFORM_KE02_UART_HPP_
