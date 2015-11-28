#include <target/spi.hpp>
#include <target/pinconfig.hpp>
#include <target/gpio.hpp>
#include <dev/pcd8544.hpp>
#include <dev/sdspi.hpp>

#include <functional>
#include <utility>

#include <FreeRTOS.h>
#include <task.h>
#include <ecl/assert.hpp>
#include <ecl/slab.hpp>
#include <ecl/memory.hpp>

#include "sprite.hpp"

// allocator test object
struct dummy
{
    dummy() :obj{0xbe} { ecl::cout << "dummy ctor" << ecl::endl; }
    ~dummy() { ecl::cout << "dummy dtor" << ecl::endl; }
    uint8_t obj;
};

uint8_t slab_buf[25];
ecl::slab< dummy > slab(slab_buf, sizeof(slab_buf));
ecl::slab_allocator< dummy > allocator(&slab);

// Allocator test itself
void test_alloc(size_t sz)
{
    dummy *ptr = allocator.allocate(sz);
    ecl::cout << "Sz: " << sz << " ptr: " << (int) ptr << ecl::endl;
    assert(ptr >= (dummy*)slab_buf && ptr < (dummy*)(slab_buf + sizeof(slab_buf)));
}

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

    char c = 0;
    int ret = 0;

    ecl::cout << "\n\nHello, embedded world!" << ecl::endl;
    // TODO: order of initialization must be preserved
    // as soon as default values for GPIO will be introduced
    sd_spi< SPI_LCD_driver,  SDSPI_CS > sdspi;
    pcd8544< SPI_LCD_driver > lcd;

    sdspi.init();
    ret = sdspi.open();

    lcd.init();
    lcd.open();
    lcd.clear();
    lcd.flush();

    if (!ret) {
        uint8_t buf[512];

        for (uint i = 0; i < 1; ++i) {
            if (sdspi.read(buf, sizeof(buf)) != sizeof(buf)) {
                ecl::cout << "Unable to retrieve buffer" << ecl::endl;
            } else {
                ecl::cout << "items: ";
                for (uint16_t i = 0; i < sizeof(buf); ++i) {
                    ecl::cout << buf[i] << " ";
                    if ((i & 0xf) == 0xf)
                        ecl::cout << ecl::endl;
                }
                ecl::cout << ecl::endl;
            }
        }

        sdspi.seek(510);
        uint8_t buf2[] = {1, 2, 3, 4, 5, 6};
        sdspi.write(buf2, sizeof(buf2));
        sdspi.flush();
        ecl::cout << "=======================" << ecl::endl;

        sdspi.seek(510);

        if (sdspi.read(buf, sizeof(buf)) != sizeof(buf)) {
            ecl::cout << "Unable to retrieve buffer" << ecl::endl;
        } else {
            ecl::cout << "items: ";
            for (uint16_t i = 0; i < sizeof(buf); ++i) {
                ecl::cout << buf[i] << " ";
                if ((i & 0xf) == 0xf)
                    ecl::cout << ecl::endl;
            }
            ecl::cout << ecl::endl;
        }

    }

    ecl::cout << "Starting..." << ecl::endl;
    ecl::cout << "Sizeof cache: " << sizeof(dummy) << ecl::endl;
    ecl::cout << "Sizeof slab: " << sizeof(slab_buf) << ecl::endl;
    ecl::cout << "Alignof cache: " << alignof(dummy) << ecl::endl;
    ecl::cout << "Slab start " << (int)slab_buf << ecl::endl;
    ecl::cout << "Slab end " << (int)(slab_buf + sizeof(slab_buf)) << ecl::endl;

    test_alloc(5);
    test_alloc(1);
    test_alloc(3);
    test_alloc(1);
    test_alloc(1);
    test_alloc(10);
    test_alloc(1);
    // This will trigger assert, since there is no more memory in buffer
    //test_alloc(1);

    for (;;) {
        ecl::cin >> c;

        if (c != '\033') { // Escape sequence
            ecl::cout << c;
        }

        switch (c) {
        case '\r':
            ecl::cout << "\n$ ";
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
            ecl::cin >> c;
            ecl::cin >> c;

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
    // Let the fun begin!

    xTaskCreate(rtos_task0, "task0", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(rtos_task1, "task1", 1024, NULL, tskIDLE_PRIORITY, NULL);

    vTaskStartScheduler();

    return 0;
}
