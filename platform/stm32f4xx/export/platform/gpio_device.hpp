#ifndef PLATFORM_GPIO_DEVICE_HPP
#define PLATFORM_GPIO_DEVICE_HPP

#include <sys/pin_cfgs.h>
#include "pin_descriptor.hpp"

// TODO: support for output port as well
template< PinPort port, PinNum pin >
class GPIO
{
public:
	constexpr GPIO();

	// Set pin to 1
	constexpr void set() const;

	// Set pin to 0
	constexpr void reset() const;

	// Get input data
	uint8_t get() const;
};

template< PinPort port, PinNum pin >
constexpr GPIO< port, pin >::GPIO()
{

}

template< PinPort port, PinNum pin >
constexpr void GPIO< port, pin >::set() const
{
	constexpr auto m_port = pickPort(port);
	constexpr auto m_pin  = pickPin(pin);

	GPIO_WriteBit(m_port, m_pin, Bit_SET);
}

template< PinPort port, PinNum pin >
constexpr void GPIO< port, pin >::reset() const
{
	constexpr auto m_port = pickPort(port);
	constexpr auto m_pin  = pickPin(pin);

	GPIO_WriteBit(m_port, m_pin, Bit_RESET);
}

template< PinPort port, PinNum pin >
uint8_t GPIO< port, pin >::get() const
{
	constexpr auto m_port = pickPort(port);
	constexpr auto m_pin  = pickPin(pin);

	return static_cast< uint8_t >(GPIO_ReadInputDataBit(m_port, m_pin));
}


#endif
