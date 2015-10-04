#include <target/usart.hpp>
#include <target/spi.hpp>
#include <target/pinconfig.hpp>
#include <target/gpio.hpp>
#include <platform/irq_manager.hpp>

#include <dev/pcd8544.hpp>

#include <functional>
#include <utility>


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

void _delay()
{
    for (volatile int i = 0; i < 10000; ++i) {};
}

int main()
{
    //dasdasdsadd = 0;
    console_driver console;

    // TODO: move it to a better place
    IRQ_manager::init();

    PCD8544< SPI_LCD_driver > lcd;
    lcd.init();
    lcd.open();
    lcd.clear();

    console.init();
    console.open();

    uint8_t c;

    console.write((uint8_t *)"$", 1);
    console.write((uint8_t *)" ", 1);

    /* For validation */
    for (size_t i = 0; i < sizeof(static_array); ++i) {
        console.write(&static_array[i], 1);
    }

    /* For validation */
    for (size_t i = 0; i < sizeof(static_array2); ++i) {
        console.write((uint8_t *)&static_array2[i], 1);
    }

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

            static int d = 0;
            for (int i = 0; i < 48; ++i) {
                point p{i + d, i};
                lcd.set_point(p);
            }

            d++;
            lcd.flush();

            _delay();

            for (int i = 0; i < 48; ++i) {
                point p{i + d, i};
                lcd.set_point(p);
            }
            lcd.flush();

            break;
        default:
            break;
        }
    }

    return 0;
}
