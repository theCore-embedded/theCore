#include "pinconfig.hpp"
#include "pin_descr.hpp"

void initalizePins()
{
	PinDescr toInit[] = {
		// UART1
		createPin(PinPort::PORT_A, PinNum::PIN_8, PinAssignment::UART1, PinType::NOPULL),
		createPin(PinPort::PORT_A, PinNum::PIN_9, PinAssignment::UART1, PinType::NOPULL),
		createPin(PinPort::PORT_A, PinNum::PIN_10, PinAssignment::UART1, PinType::NOPULL),
		createPin(PinPort::PORT_A, PinNum::PIN_11, PinAssignment::UART1, PinType::NOPULL),
		createPin(PinPort::PORT_A, PinNum::PIN_12, PinAssignment::UART1, PinType::NOPULL),
	};

	for (auto pin : toInit) {
		pin.init();
	}
}
