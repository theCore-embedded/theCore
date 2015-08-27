#include <Usart.hpp>
#include <pinconfig.hpp>

int main()
{
	initializePins();

	ConsoleDriver console;
	console.init();

	uint8_t c;

	for (;;) {
		console.read(&c, 1);
		console.write(&c, 1);
	}

	return 0;
}
