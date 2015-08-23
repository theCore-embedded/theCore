#ifndef PLATFORM_PIN_DESCR_HPP
#define PLATFORM_PIN_DESCR_HPP

#include "pin_cfgs.h"

#include <stm32f4xx_gpio.h>

// Pin descriptor, used for pinconfig
struct PinDescr
{
	// TODO: add APB here
	const GPIO_TypeDef       *port;
	const uint32_t           pin;
	const GPIOMode_TypeDef   mode;
	const GPIOSpeed_TypeDef  speed;
	const GPIOOType_TypeDef  type;
	const GPIOPuPd_TypeDef   pupd;
	const uint8_t            af;

	// Inits a pin
	void init() const
	{
		// TODO: create it
		return;
	}
};



// Possible pin combinations, according to the datasheet

//template <>
//constexpr PinDescr obtainPin<	PinPort			::PORT_D,
//								PinNum			::PIN_9,
//								PinAssignment	::UART3,
//								PinType			::NOPULL >()
//{
//	return PinDescr {
//				GPIOD,
//				GPIO_Pin_9,
//				GPIO_Mode_AF,
//				GPIO_Medium_Speed,
//				GPIO_OType_PP,
//				GPIO_PuPd_NOPULL
//	};
//}

static constexpr GPIO_TypeDef *pickPort(const PinPort &port)
{
	switch (port) {
	case PinPort::PORT_A:
		return GPIOA;
	case PinPort::PORT_B:
		return GPIOB;
	case PinPort::PORT_C:
		return GPIOC;
	case PinPort::PORT_D:
		return GPIOD;
	default:
		// TODO: assert here!
		return GPIOA;
	}
}

// TODO: implement
static constexpr uint32_t pickPin(const PinNum &pin)
{
	return GPIO_Pin_9;
}

// TODO: implement
static constexpr GPIOMode_TypeDef pickMode(const PinAssignment &purpose)
{
	return GPIO_Mode_AF;
}

// TODO: implement
static constexpr GPIOSpeed_TypeDef pickSpeed()
{
	return GPIO_Medium_Speed;
}

// TODO: implement
static constexpr GPIOOType_TypeDef pickType(const PinAssignment &purpose)
{
	return GPIO_OType_PP;
}

// TODO: implement
static constexpr GPIOPuPd_TypeDef pickPullType(const PinType &type)
{
	return GPIO_PuPd_NOPULL;
}

// TODO: implement
static constexpr uint8_t pickAf(const PinAssignment &purpose)
{
	return GPIO_AF_USART1;
}


constexpr PinDescr createPin(const PinPort &port,
				   const PinNum &pinnum,
				   const PinAssignment &purpose,
				   const PinType &type
				   )
{
	return PinDescr {
		pickPort(port),
		pickPin(pinnum),
		pickMode(purpose),
		pickSpeed(),
		pickType(purpose),
		pickPullType(type),
		pickAf(purpose)
	};
}

// Pin map itself
// TODO: extend
MAP_ENTRY(PinPort::PORT_D, PinNum::PIN_9, PinAssignment::UART1);
MAP_ENTRY(PinPort::PORT_A, PinNum::PIN_9, PinAssignment::UART1);

#endif // PLATFORM_PIN_DESCR_HPP

