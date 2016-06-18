//! \file
//! \brief GPIO device interface for stm32f4xx platform
//! \todo Detailed explanation.
#ifndef PLATFORM_GPIO_DEVICE_HPP
#define PLATFORM_GPIO_DEVICE_HPP

#include "aux/pin_descriptor.hpp"

namespace ecl
{

// Encapsulates pin usage
//! GPIO class incapsulates pin usage.
//! \tparam Port GPIO port on stm32f4xx.
//! \tparam Pin  Pin within given port.
template< gpio_port Port, gpio_num Pin >
class gpio
{
public:
    static constexpr auto port = Port;
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

template< gpio_port Port, gpio_num Pin >
void gpio< Port, Pin >::set()
{
    constexpr auto hw_port = pick_port(Port);
    constexpr auto hw_pin  = pick_pin(Pin);

    GPIO_WriteBit(hw_port, hw_pin, Bit_SET);
}

template< gpio_port Port, gpio_num Pin >
void gpio< Port, Pin >::reset()
{
    constexpr auto hw_port = pick_port(Port);
    constexpr auto hw_pin  = pick_pin(Pin);

    GPIO_WriteBit(hw_port, hw_pin, Bit_RESET);
}

template< gpio_port Port, gpio_num Pin >
void gpio< Port, Pin >::toggle()
{
    constexpr auto hw_port = pick_port(Port);
    constexpr auto hw_pin  = pick_pin(Pin);

    GPIO_ToggleBits(hw_port, hw_pin);
}

template< gpio_port Port, gpio_num Pin >
bool gpio< Port, Pin >::get()
{
    constexpr auto hw_port = pick_port(Port);
    constexpr auto hw_pin  = pick_pin(Pin);

    return GPIO_ReadInputDataBit(hw_port, hw_pin) == Bit_SET;
}

} // namespace ecl

#endif
