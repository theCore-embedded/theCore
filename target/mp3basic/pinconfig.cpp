#include <cstddef>
#include "pinconfig.hpp"
#include "pin_descr.hpp"

extern "C" {
// HACK - move it somewhere
void *memcpy(uint8_t *dst, const uint8_t *src, size_t cnt)
{
	while (cnt--) {
		*dst++ = *src++;
	}

	return dst;
}
}

void initializePins()
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
