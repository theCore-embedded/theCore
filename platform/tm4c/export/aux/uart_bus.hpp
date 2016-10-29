//! \file
//! \brief UART implementation for TI TM4C MCU

#ifndef PLATFORM_TM4C_UART_BUS_HPP_
#define PLATFORM_TM4C_UART_BUS_HPP_

#include <common/bus.hpp>

#include <uart.h>
#include <hw_memmap.h>

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
public:
    //! \brief Lazy initialization.
    //! \return Status of operation.
    static ecl::err init();

    //! \brief Sets rx buffer with given size.
    //! \param[in,out]  rx      Buffer to write data to. Optional.
    //! \param[in]      size    Size
    static void set_rx(uint8_t *rx, size_t size)
    { (void) rx; (void) size; }

    //! \brief Sets rx buffer made-up from sequence of similar bytes.
    //! \param[in] size         Size of sequence
    //! \param[in] fill_byte    Byte to fill a sequence. Optional.
    static void set_tx(size_t size, uint8_t fill_byte = 0xff)
    { (void) size; (void) fill_byte; }

    //! \brief Sets tx buffer with given size.
    //! \param[in] tx   Buffer to transmit. Optional.
    //! \param[in] size Buffer size.
    static void set_tx(const uint8_t *tx, size_t size)
    { (void) tx; (void) size; }

    //! \brief Sets event handler.
    //! Handler will be used by the bus, until reset_handler() will be called.
    //! \param[in] handler Handler itself.
    static void set_handler(const bus_handler &handler)
    { (void) handler; }

    //! \brief Reset xfer buffers.
    //! Buffers that were set by \sa set_tx() and \sa set_rx()
    //! will be no longer used after this call.
    static void reset_buffers()
    { return; }

    //! \brief Resets previously set handler.
    static void reset_handler()
    { return; }

    //! \brief Executes xfer, using buffers previously set.
    //! When it will be done, handler will be invoked.
    //! \return Status of operation.
    static ecl::err do_xfer()
    { return ecl::err::nosys; }
};

//------------------------------------------------------------------------------

template<uart_device dev>
ecl::err uart_bus<dev>::init()
{
    return err::ok;
}

} // namespace ecl

#endif // PLATFORM_TM4C_UART_BUS_HPP_
