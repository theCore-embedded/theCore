#include "target.hpp"

#include <ecl/iostream.hpp>
#include <ecl/thread/utils.hpp>
#include <ecl/thread/semaphore.hpp>

// Audio file in a form of C array
#include "audio_u8.bin"

using ecl::cout;
using ecl::endl;
using ecl::semaphore;

extern "C"
void board_init()
{
    gpio_init();
}

int main()
{
    ecl::cout << "Starting audio play..." << ecl::endl;
    return 0;

}
