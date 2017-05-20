//! \file
//! \brief Module with handlers for UART Electron events

#include "aux/uart_bus.hpp"


//! Carries events to particular UART theCore driver.
template<ecl::uart_device dev>
struct uart_particle_carrier : ecl::uart_bus<dev>
{
    static void handle_event()
    {
        ecl::uart_bus<dev>::serial_handler();
    }
};


#ifdef THECORE_USE_SERIAL0
void serialEvent()
{
    uart_particle_carrier<ecl::uart_device::serial0>::handle_event();
}
#endif

#ifdef THECORE_USE_SERIAL1
void serialEvent1()
{
    uart_particle_carrier<ecl::uart_device::serial1>::handle_event();
}
#endif

#ifdef THECORE_USE_SERIAL2
void serialEvent2()
{
    uart_particle_carrier<ecl::uart_device::serial2>::handle_event();
}
#endif

#ifdef THECORE_USE_SERIAL4
void serialEvent4()
{
    uart_particle_carrier<ecl::uart_device::serial4>::handle_event();
}
#endif

#ifdef THECORE_USE_SERIAL5
void serialEvent5()
{
    uart_particle_carrier<ecl::uart_device::serial5>::handle_event();
}
#endif
