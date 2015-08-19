#ifndef SYS_PIN_CFGS_H
#define SYS_PIN_CFGS_H

// Pin purpose
enum class PinAssignment
{
	// Basic purposes
	GPIO_IN,
	GPIO_OUT,
	ANALOG,

	// Alternate function purposes
	PIN_SPI1,
	PIN_SPI2,
	PIN_SPI3,
	UART1,
	UART2,
	UART3,
};

// Pin numbers
enum class PinNum
{
	PIN_0,
	PIN_1,
	PIN_2,
	PIN_3,
	PIN_4,
	PIN_5,
	PIN_6,
	PIN_7,
	PIN_8,
	PIN_9,
};

// Pin ports
enum class PinPort
{
	PORT_A,
	PORT_B,
	PORT_C,
	PORT_D,
	PORT_E,
};

// Type of pin
enum class PinType {
	NOPULL,
	PULL_UP,
	PULL_DOWN,
	OPEN_DRAIN
};

// Type is defined by platform
struct PinDescr;

// Obtains proper pin using pin purpose
// TODO: extend speed here
template <PinPort port, PinNum pin, PinAssignment purpose, PinType type>
constexpr PinDescr obtainPin();

#endif
