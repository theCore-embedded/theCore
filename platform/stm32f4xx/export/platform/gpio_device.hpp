#ifndef PLATFORM_GPIO_DEVICE_HPP
#define PLATFORM_GPIO_DEVICE_HPP

#include <common/pin.hpp>
#include "pin_descriptor.hpp"

// TODO: support for output port as well
// Incapsulate pin usage
template< pin::port port, pin::number pin >
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

template< pin::port port, pin::number pin >
void GPIO< port, pin >::set()
{
    constexpr auto m_port = pick_port(port);
    constexpr auto m_pin  = pick_pin(pin);

    GPIO_WriteBit(m_port, m_pin, Bit_SET);
}

template< pin::port port, pin::number pin >
void GPIO< port, pin >::reset()
{
    constexpr auto m_port = pick_port(port);
    constexpr auto m_pin  = pick_pin(pin);

    GPIO_WriteBit(m_port, m_pin, Bit_RESET);
}

template< pin::port port, pin::number pin >
void GPIO< port, pin >::toggle()
{
    constexpr auto m_port = pick_port(port);
    constexpr auto m_pin  = pick_pin(pin);

    GPIO_ToggleBits(m_port, m_pin);
}

template< pin::port port, pin::number pin >
uint8_t GPIO< port, pin >::get()
{
    constexpr auto m_port = pick_port(port);
    constexpr auto m_pin  = pick_pin(pin);

    return static_cast< uint8_t >(GPIO_ReadInputDataBit(m_port, m_pin));
}


#endif
