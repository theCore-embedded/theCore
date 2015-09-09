// TODO: consider using better name
#ifndef PLATFORM_PIN_DESCR_HPP
#define PLATFORM_PIN_DESCR_HPP

#include <sys/pin_cfgs.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>

// Pin descriptor, used for pinconfig
struct PinDescr
{
	// TODO: add APB here
	GPIO_TypeDef             *port;
	const uint32_t           pin;
	const GPIOMode_TypeDef   mode;
	const GPIOSpeed_TypeDef  speed;
	const GPIOOType_TypeDef  type;
	const GPIOPuPd_TypeDef   pupd;
	const uint32_t           ahb;
	const uint8_t            src;
	const uint8_t            af;

	// Inits a pin
	// TODO: move it to the CPP file
	void init()
	{
		GPIO_InitTypeDef initStruct;

		RCC_AHB1PeriphClockCmd(this->ahb, ENABLE);
		GPIO_StructInit(&initStruct);

		initStruct.GPIO_Pin    = this->pin;
		initStruct.GPIO_Mode   = this->mode;
		initStruct.GPIO_Speed  = this->speed;
		initStruct.GPIO_OType  = this->type;
		initStruct.GPIO_PuPd   = this->pupd;

		GPIO_Init(this->port, &initStruct);

		if (this->mode == GPIO_Mode_AF) {
			GPIO_PinAFConfig(this->port, this->src, this->af);
		}
	}
};

// TODO: implement, comments
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
	case PinPort::PORT_E:
		return GPIOE;
	default:
		return nullptr;
	}
}

// TODO: implement, comments
static constexpr uint32_t pickPin(const PinNum &pin)
{
	switch (pin) {
	case PinNum::PIN_0:
		return GPIO_Pin_0;
	case PinNum::PIN_1:
		return GPIO_Pin_1;
	case PinNum::PIN_2:
		return GPIO_Pin_2;
	case PinNum::PIN_3:
		return GPIO_Pin_3;
	case PinNum::PIN_4:
		return GPIO_Pin_4;
	case PinNum::PIN_5:
		return GPIO_Pin_5;
	case PinNum::PIN_6:
		return GPIO_Pin_6;
	case PinNum::PIN_7:
		return GPIO_Pin_7;
	case PinNum::PIN_8:
		return GPIO_Pin_8;
	case PinNum::PIN_9:
		return GPIO_Pin_9;
	case PinNum::PIN_10:
		return GPIO_Pin_10;
	case PinNum::PIN_11:
		return GPIO_Pin_11;
	case PinNum::PIN_12:
		return GPIO_Pin_12;
	case PinNum::PIN_13:
		return GPIO_Pin_13;
	case PinNum::PIN_14:
		return GPIO_Pin_14;
	case PinNum::PIN_15:
		return GPIO_Pin_15;
	default:
		return (uint32_t) -1;
	}
}

// TODO: implement, comments
static constexpr uint8_t pickSrc(const PinNum &pin)
{
	switch (pin) {
	case PinNum::PIN_0:
		return GPIO_PinSource0;
	case PinNum::PIN_1:
		return GPIO_PinSource1;
	case PinNum::PIN_2:
		return GPIO_PinSource2;
	case PinNum::PIN_3:
		return GPIO_PinSource3;
	case PinNum::PIN_4:
		return GPIO_PinSource4;
	case PinNum::PIN_5:
		return GPIO_PinSource5;
	case PinNum::PIN_6:
		return GPIO_PinSource6;
	case PinNum::PIN_7:
		return GPIO_PinSource7;
	case PinNum::PIN_8:
		return GPIO_PinSource8;
	case PinNum::PIN_9:
		return GPIO_PinSource9;
	case PinNum::PIN_10:
		return GPIO_PinSource10;
	case PinNum::PIN_11:
		return GPIO_PinSource11;
	case PinNum::PIN_12:
		return GPIO_PinSource12;
	case PinNum::PIN_13:
		return GPIO_PinSource13;
	case PinNum::PIN_14:
		return GPIO_PinSource14;
	case PinNum::PIN_15:
		return GPIO_PinSource15;
	default:
		return (uint8_t) -1;
	}
}

// TODO: implement, comments
static constexpr GPIOMode_TypeDef pickMode(const PinAssignment &purpose)
{
	switch (purpose) {
	case PinAssignment::GPIO_IN:
		return GPIO_Mode_IN;
	case PinAssignment::GPIO_OUT:
		return GPIO_Mode_OUT;
	case PinAssignment::ANALOG:
		return GPIO_Mode_AN;
	default:
		return GPIO_Mode_AF;
	}

}

// TODO: implement, comments
static constexpr GPIOSpeed_TypeDef pickSpeed()
{
	return GPIO_Medium_Speed;
}

// TODO: implement, comments
static constexpr GPIOOType_TypeDef pickType(const PinType &type)
{
	if (type == PinType::OPEN_DRAIN) {
		return GPIO_OType_OD;
	} else {
		return GPIO_OType_PP;
	}
}

// TODO: implement, comments
static constexpr GPIOPuPd_TypeDef pickPullType(const PinType &type)
{
	switch (type) {
	case PinType::PULL_DOWN:
		return GPIO_PuPd_DOWN;
	case PinType::PULL_UP:
		return GPIO_PuPd_UP;
	default:
		return GPIO_PuPd_NOPULL;
	}
}

// TODO: implement, comments
static constexpr uint8_t pickAf(const PinAssignment &purpose)
{
	switch (purpose) {
	case PinAssignment::UART1:
		return GPIO_AF_USART1;
	case PinAssignment::UART2:
		return GPIO_AF_USART2;
	case PinAssignment::UART3:
		return GPIO_AF_USART3;
		// TODO: add rest of usarts
	case PinAssignment::PIN_SPI1:
		return GPIO_AF_SPI1;
	case PinAssignment::PIN_SPI2:
		return GPIO_AF_SPI2;
	case PinAssignment::PIN_SPI3:
		return GPIO_AF_SPI3;
		// TODO: add rest of SPI
	default:
		return (uint8_t) -1;
	}
}

static constexpr uint32_t pickPeriph(const PinPort &port)
{
	switch (port) {
	case PinPort::PORT_A:
		return RCC_AHB1Periph_GPIOA;
	case PinPort::PORT_B:
		return RCC_AHB1Periph_GPIOB;
	case PinPort::PORT_C:
		return RCC_AHB1Periph_GPIOC;
	case PinPort::PORT_D:
		return RCC_AHB1Periph_GPIOD;
	case PinPort::PORT_E:
		return RCC_AHB1Periph_GPIOE;
	default:
		return (uint32_t) -1;
	}
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
		pickType(type),
		pickPullType(type),
		pickPeriph(port),
		pickSrc(pinnum),
		pickAf(purpose),
	};
}


#if 0
// Possible pin combinations, according to the datasheet
// TODO: extend

// TODO: is needed?
// USART1
MAP_ENTRY(PinPort::PORT_A, PinNum::PIN_8, PinAssignment::UART1);
MAP_ENTRY(PinPort::PORT_A, PinNum::PIN_9, PinAssignment::UART1);
MAP_ENTRY(PinPort::PORT_A, PinNum::PIN_10, PinAssignment::UART1);
MAP_ENTRY(PinPort::PORT_A, PinNum::PIN_11, PinAssignment::UART1);
MAP_ENTRY(PinPort::PORT_A, PinNum::PIN_12, PinAssignment::UART1);
MAP_ENTRY(PinPort::PORT_B, PinNum::PIN_6, PinAssignment::UART1);
MAP_ENTRY(PinPort::PORT_B, PinNum::PIN_7, PinAssignment::UART1);

// USART2
MAP_ENTRY(PinPort::PORT_A, PinNum::PIN_0, PinAssignment::UART2);
MAP_ENTRY(PinPort::PORT_A, PinNum::PIN_1, PinAssignment::UART2);
MAP_ENTRY(PinPort::PORT_A, PinNum::PIN_2, PinAssignment::UART2);
MAP_ENTRY(PinPort::PORT_A, PinNum::PIN_3, PinAssignment::UART2);
MAP_ENTRY(PinPort::PORT_A, PinNum::PIN_4, PinAssignment::UART2);
MAP_ENTRY(PinPort::PORT_D, PinNum::PIN_3, PinAssignment::UART2);
MAP_ENTRY(PinPort::PORT_D, PinNum::PIN_4, PinAssignment::UART2);
MAP_ENTRY(PinPort::PORT_D, PinNum::PIN_5, PinAssignment::UART2);
MAP_ENTRY(PinPort::PORT_D, PinNum::PIN_6, PinAssignment::UART2);
MAP_ENTRY(PinPort::PORT_D, PinNum::PIN_7, PinAssignment::UART2);
#endif

#endif // PLATFORM_PIN_DESCR_HPP

