//! \file
//! \brief TM4C GPIO device.
#ifndef TM4C_GPIO_DEVICE_HPP_
#define TM4C_GPIO_DEVICE_HPP_

#include "aux/pin_cfg.hpp"

#include <gpio.h>

namespace ecl
{

//! Encapsulates pin usage.
//! \tparam Port GPIO port on STM32 device.
//! \tparam Pin  Pin within given port.
template<gpio_hw::port Port, gpio_hw::num Pin>
class gpio
{
public:
    //! Advertised platform port.
    static constexpr auto port = Port;

    //! Advertised platform pin.
    static constexpr auto pin = Pin;

    //! Sets output pin level to high.
    static void set();

    //! Sets output pin level to low.
    static void reset();

    //! Toggles pin level.
    static void toggle();

    //! Gets level on a pin.
    //! \return True if level is high, false if level is low.
    static bool get();
};

//------------------------------------------------------------------------------

template<gpio_hw::port Port, gpio_hw::num Pin>
void gpio<Port, Pin>::set()
{
    auto port = static_cast<std::underlying_type_t<gpio_hw::port>>(Port);
    auto pin  = static_cast<std::underlying_type_t<gpio_hw::num>>(Pin);

    GPIOPinWrite(port, pin, pin);
}

template<gpio_hw::port Port, gpio_hw::num Pin>
void gpio<Port, Pin>::reset()
{
    auto port = static_cast<std::underlying_type_t<gpio_hw::port>>(Port);
    auto pin  = static_cast<std::underlying_type_t<gpio_hw::num>>(Pin);

    GPIOPinWrite(port, pin, 0);
}

template<gpio_hw::port Port, gpio_hw::num Pin>
void gpio<Port, Pin>::toggle()
{
    auto port = static_cast<std::underlying_type_t<gpio_hw::port>>(Port);
    auto pin  = static_cast<std::underlying_type_t<gpio_hw::num>>(Pin);

    auto pin_val = GPIOPinRead(port, pin);
    pin_val ^= pin;
    GPIOPinWrite(port, pin, pin_val);
}

template<gpio_hw::port Port, gpio_hw::num Pin>
bool gpio<Port, Pin>::get()
{
    auto port = static_cast<std::underlying_type_t<gpio_hw::port>>(Port);
    auto pin  = static_cast<std::underlying_type_t<gpio_hw::num>>(Pin);

    auto pin_val = GPIOPinRead(port, pin);
    return pin_val != 0;
}

} // namespace ecl

#endif // TM4C_GPIO_DEVICE_HPP_
