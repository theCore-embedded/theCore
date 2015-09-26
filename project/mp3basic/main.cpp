#include <target/usart.hpp>
#include <target/spi.hpp>
#include <target/pinconfig.hpp>
#include <target/gpio.hpp>

/* For validation */
static uint8_t static_array[] =
{
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
};

/* For validation */
static char static_array2[] =
{
	'H', 'E', 'L', 'L', '\r', '\n',
};

/* For validation */
class FooBar
{
public:
	FooBar()
	{
		m_console.init();
		m_console.open();
		static char another_arr[] = "Vini Vidi Vici\n\r";

		for (size_t i = 0; i < sizeof(another_arr); ++i) {
			m_console.write((uint8_t *)&another_arr[i], 1);
		}

		m_console.close();
	}

private:
	ConsoleDriver m_console;
};

FooBar foo;

int main()
{
	ConsoleDriver console;

	console.init();
	console.open();

	(void) foo;

	uint8_t c;

	PCD8544_CS::set();
	PCD8544_Reset::set();
	PCD8544_Mode::set();


	console.write((uint8_t *)"$", 1);
	console.write((uint8_t *)" ", 1);

	SpiLCDDriver spi(SpiDirection::TX,
					 SpiMode::MASTER,
					 SpiCPOL::HIGH,
					 SpiCPHA::SECOND_EDGE,
					 SpiNssType::SW,
					 SpiBitOrder::MSB,
					 0);
	spi.init();
	spi.open();

	auto _delay = []() {
		for (volatile int i = 0; i < 10000; ++i) {};
	};

	/* For validation */
	for (size_t i = 0; i < sizeof(static_array); ++i) {
		console.write(&static_array[i], 1);
	}

	/* For validation */
	for (size_t i = 0; i < sizeof(static_array2); ++i) {
		console.write((uint8_t *)&static_array2[i], 1);
	}

	auto _send = [&spi, _delay] (uint8_t byte, uint8_t op) {

		if (op)
			PCD8544_Mode::set();
		else
			PCD8544_Mode::reset();

		PCD8544_CS::reset();
		spi.write(&byte, 1);
		PCD8544_CS::set();
	};

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
		case 'l': // LCD op
			// RESET
			PCD8544_Reset::reset();
			_delay();
			PCD8544_Reset::set();

			// found somewhere in net
			_send(0x21, 0);
			_send(0xc6, 0);
			_send(0x06, 0);
			_send(0x13, 0);
			_send(0x20, 0);

			// clear
			for (unsigned i = 0; i < 6 * 84; ++i) {
				_send(0x0, 1);
			}

			_send(0x0c, 0);

			// write something
			for (unsigned i = 0; i < 6 * 84; ++i) {
				_send((uint8_t) i, 1);
			}

			_delay();
			break;
		default:
			break;
		}
	}

	return 0;
}
