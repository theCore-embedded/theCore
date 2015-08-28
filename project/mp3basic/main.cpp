#include <Usart.hpp>
#include <pinconfig.hpp>

int main()
{
	initializePins();

	ConsoleDriver console;
	console.init();
	console.open();

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
