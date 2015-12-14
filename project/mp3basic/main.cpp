#include <target/spi.hpp>
#include <target/pinconfig.hpp>
#include <target/gpio.hpp>
#include <dev/pcd8544.hpp>
#include <dev/sdspi.hpp>
#include <functional>
#include <utility>

#include <fat/file_inode.hpp>
#include <fat/fs.hpp>
#include <ecl/assert.hpp>
#include <ecl/pool.hpp>
#include <fs/inode.hpp>
#include <fs/fs_descriptor.hpp>
#include <fat/fs.hpp>
#include <fs/fs.hpp>

#include <tuple>

#include "sprite.hpp"

constexpr const char fat_root[] = "/fat";
using Block = sd_spi< SPI_LCD_driver,  SDSPI_CS >;
using Fat   = fs::fs_descriptor< fat_root, fat::petit< Block > >;
static fs::vfs< Fat > fs_obj;
pcd8544< SPI_LCD_driver > lcd;

static void rtos_task0(void *params)
{
    (void) params;
    for (;;) {
        LED_Red::toggle();
        // It should blink at rate 1 Hz,
        // if not - something wrong with clock setup
        delay(1000);
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

    fs_obj.mount_all();

    std::function< void(fs::dir_ptr fd, const char *dir) > traverse;

    traverse = [&traverse](fs::dir_ptr fd, const char *dir) -> void {
        assert(fd);

        int ret = fd->rewind();
        assert(!ret);
        ecl::cout << "-------------------------" << ecl::endl;
        ecl::cout << "\nTraversing " << dir << ecl::endl;

        fs::inode_ptr node;
        char buf[16];

        while ((node = fd->next())) {
            size_t read = node->get_name(buf, sizeof(buf));
            auto type = node->get_type();
            ecl::cout << "TYPE: -> " << (int) type << " READ -> "
                      << (int) read << " NAME -> " << buf << ecl::endl;

            if (type == fs::inode::type::dir) {
                auto dd = node->open_dir();
                if (dd)
                    traverse(dd, buf);
            }
        }

        ecl::cout << "-------------------------" << ecl::endl;
        return;
    };

#if 1
    {
        auto dd = fs_obj.open_dir("/fat");
        traverse(dd, "/fat");
    }
#endif

#if 1
    {
        auto read_lambda = [](const char *name, size_t amount) {
            ecl::cout << "\n *** Reading " << name << ecl::endl;

            auto fd = fs_obj.open_file(name);
            assert(fd);

            char buf[8];
            ssize_t read = 1;

            while (read && amount--) {
                read = fd->read((uint8_t*)buf, sizeof(buf) - 1);
                assert(read >= 0);
                buf[read] = 0;
                ecl::cout << buf;
            }

            ecl::cout << ecl::endl;
        };

        read_lambda("/fat/man/makefile.in", 1000);
        read_lambda("/fat/refs/heads/master", 1000);
    }
#endif

    lcd.init();
    lcd.open();
    lcd.clear();
    lcd.flush();

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
    int ret;

#if 0
    //     Let the fun begin!
    ret = xTaskCreate(rtos_task0, "task0", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    ecl::cout << "\ntask0 ret: " << ret << ecl::endl;
    assert(ret == pdPASS);

    ret = xTaskCreate(rtos_task1, "task1", 256, NULL, tskIDLE_PRIORITY, NULL);
    ecl::cout << "task1 ret: " << ret << ecl::endl;;
    assert(ret == pdPASS);
#endif

    task_create(rtos_task0, "task0", 128);
    task_create(rtos_task1, "task1", 600);
    schedule_start();

    //rtos_task1(nullptr);

    (void) ret;
    (void) rtos_task0;
    (void) rtos_task1;

    return 0;
}
