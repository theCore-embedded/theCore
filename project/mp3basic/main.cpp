#include <target/usart.hpp>
#include <target/spi.hpp>
#include <target/pinconfig.hpp>
#include <target/gpio.hpp>
#include <platform/irq_manager.hpp>

#include <dev/pcd8544.hpp>

#include <functional>
#include <utility>

#include "sprite.hpp"

void _delay()
{
    for (volatile int i = 0; i < 100000; ++i) {};
}

int main()
{
    console_driver console;

    // TODO: move it to a better place
    IRQ_manager::init();


    PCD8544< SPI_LCD_driver > lcd;
    lcd.init();
    lcd.open();
    lcd.clear();
    lcd.flush();

    console.init();
    console.open();

    uint8_t c;

    console.write((uint8_t *)"$", 1);
    console.write((uint8_t *)" ", 1);

    for (;;) {
        console.read(&c, 1);

        if (c != '\033') { // Escape sequence
            console.write(&c, 1);
        }

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
        case '\033': // LCD op
            console.read(&c, 1); // Skip '['
            console.read(&c, 1);

            // Displacements
            static int vertical = 8;
            static int horisontal = 20;

            switch (c) {
            case 'A':
                vertical -= 2;
                break;
            case 'B':
                vertical += 2;
                break;
            case 'C':
                horisontal += 2;
                break;
            case 'D':
                horisontal -= 2;
                break;

            }

            lcd.clear();

            for (size_t i = 0; i < gimp_image.width; ++i) {
                for (size_t j = 0; j < gimp_image.height; ++j) {
                    size_t idx = (j * gimp_image.width + i) * 4;

                    if (gimp_image.pixel_data[idx + 3]) {
                        point p{static_cast< int > ((i + horisontal) % 84),
                                    static_cast< int > ((j + vertical)) & 0x3f};
                        lcd.set_point(p);
                    }
                }
            }

            lcd.flush();

            break;
        default:
            break;
        }
    }

    return 0;
}
