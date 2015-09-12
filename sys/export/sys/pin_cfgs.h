// TODO: better name
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
	PIN_10,
	PIN_11,
	PIN_12,
	PIN_13,
	PIN_14,
	PIN_15,
};

// Pin ports
enum class PinPort
{
	PORT_A,
	PORT_B,
	PORT_C,
	PORT_D,
	PORT_E,
	PORT_F,
};

// Type of pin
enum class PinType {
	NOPULL,
	FLOAT,
	PULL_UP,
	PULL_DOWN,
	OPEN_DRAIN
};

#endif
