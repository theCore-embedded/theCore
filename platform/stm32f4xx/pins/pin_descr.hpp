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

	// Inits a pin
	void init() const
	{
		// TODO: create it
		return;
	}
};

// Possible pin combinations, according to the datasheet

template <>
constexpr PinDescr obtainPin<	PinPort			::PORT_D,
								PinNum			::PIN_9,
								PinAssignment	::UART3,
								PinType			::NOPULL >()
{
	return PinDescr {
				GPIOD,
				GPIO_Pin_9,
				GPIO_Mode_AF,
				GPIO_Medium_Speed,
				GPIO_OType_PP,
				GPIO_PuPd_NOPULL
	};
}

#endif // PIN_DESCR_HPP

