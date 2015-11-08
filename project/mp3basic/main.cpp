#include <target/usart.hpp>
#include <target/spi.hpp>
#include <target/pinconfig.hpp>
#include <target/gpio.hpp>
#include <platform/irq_manager.hpp>
#include <platform/dma_device.hpp>
#include <dev/pcd8544.hpp>
#include <dev/sdspi.hpp>

#include <functional>
#include <utility>

#include "sprite.hpp"

#include <FreeRTOS.h>
#include <task.h>


static void rtos_task0(void *params)
{
    (void) params;
    for (;;) {
        LED_Red::toggle();
        // It should blink at rate 1 Hz,
        // if not - something wrong with clock setup
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static void rtos_task1(void *params)
{
    (void) params;

    uint8_t c = 0;
    int ret = 0;

    PCD8544< SPI_LCD_driver > lcd;
    lcd.init();
    lcd.open();
    lcd.clear();
    lcd.flush();

    SD_SPI< SPI_LCD_driver,  SDSPI_CS > sdspi;
    sdspi.init();

    console_driver::init();
    console_driver::open();


    for (;;) {
        console_driver::read(&c, 1);

        if (c != '\033') { // Escape sequence
            console_driver::write(&c, 1);
        }

        switch (c) {
        case '\r':
            write_string("\n$ ");
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
            console_driver::read(&c, 1); // Skip '['
            console_driver::read(&c, 1);

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

            ret = 0;

            if (lcd.clear() < 0)
                break;


            for (size_t i = 0; i < gimp_image.width && ret != -2; ++i) {
                for (size_t j = 0; j < gimp_image.height && ret != -2; ++j) {
                    size_t idx = (j * gimp_image.width + i) * 4;

                    if (gimp_image.pixel_data[idx + 3] > 0x7f) {
                        point p{static_cast< int > ((i + horisontal) % 84),
                                    static_cast< int > ((j + vertical)) & 0x3f};
                        ret = lcd.set_point(p);
                    }
                }
            }

            if (ret != -2) {
                lcd.flush();
            }

            break;
        default:
            break;
        }
    }
}

int main()
{
    // TODO: move it to a better place
    IRQ_manager::init();

    // Let the fun begin!

    xTaskCreate(rtos_task0, "task0", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(rtos_task1, "task1", 1024, NULL, tskIDLE_PRIORITY, NULL);

    vTaskStartScheduler();

    return 0;
}
