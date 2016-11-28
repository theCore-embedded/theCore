//! \file
//! \brief UART implementation for TI TM4C MCU

#ifndef PLATFORM_TM4C_UART_BUS_HPP_
#define PLATFORM_TM4C_UART_BUS_HPP_

#include <type_traits>

#include <common/bus.hpp>
#include <common/irq.hpp>
#include <ecl/assert.h>
#include <ecl/utils.hpp>

#include <uart.h>
#include <sysctl.h>
#include <interrupt.h>

#include <hw_memmap.h>
#include <hw_ints.h>

namespace ecl
{

//! Represents distinct UART peripheral devices
enum class uart_device
{
    dev0 = UART0_BASE,
    dev1 = UART1_BASE,
    dev2 = UART2_BASE,
    dev3 = UART3_BASE,
    dev4 = UART4_BASE,
    dev5 = UART5_BASE,
    dev6 = UART6_BASE,
    dev7 = UART7_BASE,
};

//! UART driver.
//! \tparam dev Peripheral device to use with this driver
template<uart_device dev>
class uart_bus
{
    //! Bypass console routines, partially reuse UART driver code.
    friend void bypass_console_init();
    friend void bypass_putc();
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

    uart_bus(const uart_bus&) = delete;
    uart_bus &operator=(uart_bus&) = delete;

private:
    //! Gets SYSCTL associated with given UART.
    static constexpr auto pick_sysctl();

    //! Gets UART interrupt number.
    static constexpr auto pick_it();

    //! Gets UART bus context.
    static constexpr auto &get_ctx();

    //! UART interrupt handler.
    static void irq_bus_handler();

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

template<uart_device dev>
safe_storage<typename uart_bus<dev>::ctx> uart_bus<dev>::m_ctx_storage;

//------------------------------------------------------------------------------
// Private members

template<uart_device dev>
constexpr auto uart_bus<dev>::pick_sysctl()
{
    switch (dev) {
        case uart_device::dev0:
            return SYSCTL_PERIPH_UART0;
        case uart_device::dev1:
            return SYSCTL_PERIPH_UART1;
        case uart_device::dev2:
            return SYSCTL_PERIPH_UART2;
        case uart_device::dev3:
            return SYSCTL_PERIPH_UART3;
        case uart_device::dev4:
            return SYSCTL_PERIPH_UART4;
        case uart_device::dev5:
            return SYSCTL_PERIPH_UART5;
        case uart_device::dev6:
            return SYSCTL_PERIPH_UART6;
        case uart_device::dev7:
            return SYSCTL_PERIPH_UART7;
    }
}

template<uart_device dev>
constexpr auto uart_bus<dev>::pick_it()
{
    switch (dev) {
        case uart_device::dev0:
            return INT_UART0;
        case uart_device::dev1:
            return INT_UART1;
        case uart_device::dev2:
            return INT_UART2;
        case uart_device::dev3:
            return INT_UART3;
        case uart_device::dev4:
            return INT_UART4;
        case uart_device::dev5:
            return INT_UART5;
        case uart_device::dev6:
            return INT_UART6;
        case uart_device::dev7:
            return INT_UART7;
    }
}

template<uart_device dev>
constexpr auto &uart_bus<dev>::get_ctx()
{
    return m_ctx_storage.get();
}

template<uart_device dev>
void uart_bus<dev>::irq_bus_handler()
{
    auto &bus_ctx = get_ctx();
    constexpr auto periph = static_cast<std::underlying_type_t<uart_device>>(dev);
    constexpr auto uart_it = pick_it();

    if (!(bus_ctx.status & ctx::tx_done)) {
        ecl_assert(UARTIntStatus(periph, true) & UART_INT_TX);

        // Handle TX termination.
        if (bus_ctx.tx_idx == bus_ctx.tx_sz) {
            bus_ctx.h(bus_channel::tx, bus_event::tc, bus_ctx.tx_sz);
            bus_ctx.status |= ctx::tx_done;
        } else {
            // Put another byte. According to the datasheet:
            // If the FIFOs are disabled (have a depth of one location) and
            // there is no data present in the transmitters single location,
            // the TXRIS bit is set.
            // It is *cleared by performing a single write to the transmit FIFO*,
            // or by clearing the interrupt by writing a 1 to the TXIC bit.

            if (bus_ctx.status & ctx::fill) {
                UARTCharPutNonBlocking(periph, bus_ctx.tx.fill_byte);
            } else {
                UARTCharPutNonBlocking(periph, bus_ctx.tx.buf[bus_ctx.tx_idx]);
            }

            ++bus_ctx.tx_idx;

            // Unblock next interrupt.

            UARTIntEnable(periph, UART_INT_TX);
        }

        // Leave TX termination to the next interrupt.

    } else if (!(bus_ctx.status & ctx::rx_done)) {
        // Half-duplex. Start RX only after TX.

        bus_ctx.rx[bus_ctx.rx_idx++] = UARTCharGetNonBlocking(periph);

        // RX interrupts won't occur anymore. Handle RX termination right now.
        if (bus_ctx.rx_idx == bus_ctx.rx_sz) {
            bus_ctx.h(bus_channel::rx, bus_event::tc, bus_ctx.rx_sz);
            bus_ctx.status |= ctx::rx_done;
        } else {
            UARTIntEnable(periph, UART_INT_RX);
        }
    }

    if ((bus_ctx.status & (ctx::tx_done | ctx::rx_done))
                         == (ctx::tx_done | ctx::rx_done)) {
        // Everything is finished.

        UARTIntClear(periph, UART_INT_TX | UART_INT_RX);
        UARTIntDisable(periph, UART_INT_TX | UART_INT_RX);
        bus_ctx.h(bus_channel::meta, bus_event::tc, 0);

        irq::mask(uart_it);
    } else {

        // Continue with next interrupts
        irq::unmask(uart_it);
    }

    irq::clear(uart_it);
}

//------------------------------------------------------------------------------

template<uart_device dev>
err uart_bus<dev>::init()
{
    auto &bus_ctx = get_ctx();

    ecl_assert(!bus_ctx.status);

    constexpr auto periph = static_cast<std::underlying_type_t<uart_device>>(dev);
    constexpr auto periph_sysctl = pick_sysctl();

    new (&m_ctx_storage) ctx;

    SysCtlPeripheralEnable(periph_sysctl);

    UARTConfigSetExpClk(periph, SysCtlClockGet(), 115200,
                        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                        UART_CONFIG_PAR_NONE);

    irq::subscribe(pick_it(), irq_bus_handler);
    UARTFIFODisable(periph);

    bus_ctx.status |= (ctx::inited | ctx::rx_done | ctx::tx_done);
    return err::ok;
}

template<uart_device dev>
void uart_bus<dev>::set_rx(uint8_t *rx, size_t size)
{
    auto &bus_ctx = get_ctx();

    ecl_assert((bus_ctx.status & (ctx::inited | ctx::rx_done))
               == (ctx::inited | ctx::rx_done));

    bus_ctx.rx = rx;
    bus_ctx.rx_sz = size;
    bus_ctx.rx_idx = 0;
}

template<uart_device dev>
void uart_bus<dev>::set_tx(const uint8_t *tx, size_t size)
{
    auto &bus_ctx = get_ctx();

    ecl_assert((bus_ctx.status & (ctx::inited | ctx::tx_done))
               == (ctx::inited | ctx::tx_done));

    bus_ctx.tx.buf = tx;
    bus_ctx.tx_sz = size;
    bus_ctx.tx_idx = 0;

    bus_ctx.status &= ~ctx::fill;
}

template<uart_device dev>
void uart_bus<dev>::set_tx(size_t size, uint8_t fill_byte)
{
    // TODO: implement
    auto &bus_ctx = get_ctx();

    ecl_assert((bus_ctx.status & (ctx::inited | ctx::tx_done))
               == (ctx::inited | ctx::tx_done));

    bus_ctx.tx.fill_byte = fill_byte;
    bus_ctx.tx_sz = size;

    bus_ctx.status |= ctx::fill;
}

template<uart_device dev>
void uart_bus<dev>::set_handler(const bus_handler &handler)
{
    auto &bus_ctx = get_ctx();

    ecl_assert((bus_ctx.status & (ctx::inited | ctx::tx_done | ctx::rx_done))
               == (ctx::inited | ctx::tx_done | ctx::rx_done));

    bus_ctx.h = handler;
}

template<uart_device dev>
void uart_bus<dev>::reset_buffers()
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

template<uart_device dev>
void uart_bus<dev>::reset_handler()
{
    auto &bus_ctx = get_ctx();

    ecl_assert((bus_ctx.status & (ctx::inited | ctx::tx_done | ctx::rx_done))
               == (ctx::inited | ctx::tx_done | ctx::rx_done));

    bus_ctx.h = stub_handler;
}

template<uart_device dev>
err uart_bus<dev>::do_xfer()
{
    auto &bus_ctx = get_ctx();

    ecl_assert((bus_ctx.status & (ctx::inited | ctx::tx_done | ctx::rx_done))
               == (ctx::inited | ctx::tx_done | ctx::rx_done));

    // At least one of direction should be enabled
    ecl_assert(bus_ctx.tx.buf || bus_ctx.rx);

    constexpr auto periph = static_cast<std::underlying_type_t<uart_device>>(dev);

    uint32_t int_flags = 0;

    if ((bus_ctx.status & ctx::fill) || bus_ctx.tx.buf) {
        int_flags |= UART_INT_TX;

        // Bus operates in half-duplex mode. TX interrupt must be provoked if TX is
        // required. Otherwise (RX required), interrupts will be triggered
        // during byte reception, no need to provoke it.
        if (bus_ctx.status & ctx::fill) {
            UARTCharPut(periph, bus_ctx.tx.fill_byte);
        } else {
            UARTCharPut(periph, bus_ctx.tx.buf[0]);
        }

        bus_ctx.tx_idx = 1;
        bus_ctx.status &= ~ctx::tx_done;
    }

    if (bus_ctx.rx) {
        int_flags |= UART_INT_RX;
        bus_ctx.status &= ~ctx::rx_done;
    }

    // Ready to go

    irq::unmask(pick_it());
    UARTIntEnable(periph, int_flags);

    return err::ok;
}

} // namespace ecl

#endif // PLATFORM_TM4C_UART_BUS_HPP_
