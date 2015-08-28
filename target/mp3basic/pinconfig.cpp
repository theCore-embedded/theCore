#include <PinDescriptor.hpp>
#include <cstddef>
#include "pinconfig.hpp"

extern "C" {
// TODO: HACK - move it somewhere
void *memcpy(uint8_t *dst, const uint8_t *src, size_t cnt)
{
	uint8_t *dest = dst;
	while (cnt--) {
		*dst++ = *src++;
	}

	return dest;
}
}

// TODO: mopve it to the target.cpp / target.h module
void initializePins()
{
	PinDescr toInit[] = {
		// USART3 TX
		createPin(PinPort::PORT_D, PinNum::PIN_8, PinAssignment::UART3, PinType::NOPULL),
		// USART3 RX
		createPin(PinPort::PORT_D, PinNum::PIN_9, PinAssignment::UART3, PinType::NOPULL),
	};

	for (auto pin : toInit) {
		pin.init();
	}
}
