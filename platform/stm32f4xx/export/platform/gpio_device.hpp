#ifndef PLATFORM_GPIO_DEVICE_HPP
#define PLATFORM_GPIO_DEVICE_HPP

#include <common/pin.hpp>
#include "pin_descriptor.hpp"

// TODO: support for output port as well
// Incapsulate pin usage
template< pin_port port, pin_number pin >
class GPIO
{
public:
    // Set pin to 1
    static void set();

    // Set pin to 0
    static void reset();

    // Toggle pin
    static void toggle();

    // Get input data
    static uint8_t get();
};

template< pin_port port, pin_number pin >
void GPIO< port, pin >::set()
{
    constexpr auto m_port = pick_port(port);
    constexpr auto m_pin  = pick_pin(pin);

    GPIO_WriteBit(m_port, m_pin, Bit_SET);
}

template< pin_port port, pin_number pin >
void GPIO< port, pin >::reset()
{
    constexpr auto m_port = pick_port(port);
    constexpr auto m_pin  = pick_pin(pin);

    GPIO_WriteBit(m_port, m_pin, Bit_RESET);
}

template< pin_port port, pin_number pin >
void GPIO< port, pin >::toggle()
{
    constexpr auto m_port = pick_port(port);
    constexpr auto m_pin  = pick_pin(pin);

    GPIO_ToggleBits(m_port, m_pin);
}

template< pin_port port, pin_number pin >
uint8_t GPIO< port, pin >::get()
{
    constexpr auto m_port = pick_port(port);
    constexpr auto m_pin  = pick_pin(pin);

    return static_cast< uint8_t >(GPIO_ReadInputDataBit(m_port, m_pin));
}


#endif
