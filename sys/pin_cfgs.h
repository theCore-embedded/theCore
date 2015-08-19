#ifndef SYS_PIN_CFGS_H
#define SYS_PIN_CFGS_H

// Pin purpose
enum class PinAssignment
{
	GPIO_IN,
	GPIO_OUT,
	ANALOG,
	SPI1,
	SPI2,
	SPI3,
	UART1,
	UART2,
	UART3,
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
template <typename PinAssignment>
constexpr PinDescr obtainPin(const PinType &type);


#endif
