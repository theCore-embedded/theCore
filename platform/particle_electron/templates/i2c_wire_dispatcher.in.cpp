//! \file
//! \brief I2C event dispatcher.

#include <aux/i2c_wire_cfgs.hpp>

namespace ecl
{

//! Real dispatch function that operates over given I2C theCore driver.
//! \tparam I2C_Dev theCore I2C driver.
//! \details Upon call, it will determine if data should be send or received
//! from the I2C driver.
template<class I2C_Dev>
static void dispatch()
{
    noInterrupts();
    if (I2C_Dev::m_status & I2C_Dev::tx_pend) {

        Wire.beginTransmission(I2C_Dev::m_addr);
        Wire.write(I2C_Dev::m_tx, I2C_Dev::m_tx_size);
        Wire.endTransmission();

        I2C_Dev::dispatcher_event_handler(bus_channel::tx, bus_event::tc);
    }
    interrupts();

    noInterrupts();
    if (I2C_Dev::m_status & I2C_Dev::rx_req) {
        auto left = I2C_Dev::m_rx_size;
        auto *buf = I2C_Dev::m_rx;

        // According to RM, Wire driver can't receive more than 32 bytes
        // at a time.
        constexpr auto recv_bytes = 32;

        Wire.beginTransmission(I2C_Dev::m_addr);

        while (left) {
            auto to_recv = left < recv_bytes ? left : recv_bytes;
            auto read = 0;

            read = Wire.requestFrom(I2C_Dev::m_addr, to_recv);
            if (!read) {
                I2C_Dev::dispatcher_event_handler(bus_channel::rx, bus_event::err);
                break;
            }

            // TODO: assert if available amount of bytes exceed requested amount.
            while (Wire.available()) {
                *buf++ = Wire.read();
            }

            left -= to_recv;
        }

        Wire.endTransmission();

        // Even if the error occur, it must be recovered and EOT must be send.
        I2C_Dev::dispatcher_event_handler(bus_channel::rx, bus_event::tc);
    }

    interrupts();
}

void i2c_dispatch()
{
    // I2C device event dispatch

    /*[[[cog
    import cog

    if 'I2C_ENABLED' in globals():
        cog.outl('dispatch<i2c_dev>();')
    ]]]*/

    //[[[end]]]
}

} // namespace ecl