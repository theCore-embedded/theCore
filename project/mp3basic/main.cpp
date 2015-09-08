#include <usart.hpp>
#include <pinconfig.hpp>
#include <spi.hpp>

int main()
{
	initializePins();

	ConsoleDriver console;
	console.init();
	console.open();

	// NOT TESTED IN HW YET!!!!
	SpiLCDDriver spi;
	spi.init();
	spi.open();

	uint8_t s;
	spi.read(&s, 1);
	spi.write(&s, 1);
	// END OF NOT TESTED

	uint8_t c;

	console.write((uint8_t *)"$", 1);
	console.write((uint8_t *)" ", 1);

	for (;;) {
		console.read(&c, 1);
		console.write(&c, 1);

		if (c == '\r') {
			console.write((uint8_t *)"\n", 1);
			console.write((uint8_t *)"$", 1);
			console.write((uint8_t *)" ", 1);
		}
	}

	return 0;
}
