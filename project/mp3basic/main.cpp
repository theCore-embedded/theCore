#include <target/usart.hpp>
#include <target/spi.hpp>
#include <target/pinconfig.hpp>
#include <target/gpio.hpp>
#include <platform/gpio_device.hpp>

int main()
{
	initializePins();

	ConsoleDriver console;
	console.init();
	console.open();

#if 0
	// NOT TESTED WITH HW YET!!!!
	SpiLCDDriver spi;
	spi.init();
	spi.open();

	uint8_t s;
	spi.read(&s, 1);
	spi.write(&s, 1);
	// END OF NOT TESTED
#endif

	uint8_t c;

	console.write((uint8_t *)"$", 1);
	console.write((uint8_t *)" ", 1);

	for (;;) {
		console.read(&c, 1);
		console.write(&c, 1);

		switch (c) {
		case '\r':
			console.write((uint8_t *)"\n", 1);
			console.write((uint8_t *)"$", 1);
			console.write((uint8_t *)" ", 1);
			break;
		case 'r':
			LED_Red::toggle();
			break;
		case 'g':
			LED_Green::toggle();
			break;
		case 'b':
			LED_Blue::toggle();
			break;
		case 'o':
			LED_Orange::toggle();
			break;
		default:
			break;
		}
	}

	return 0;
}
