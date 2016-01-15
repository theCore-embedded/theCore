#ifndef DEFAULT_CONSOLE_DRIVER_HPP_
#define DEFAULT_CONSOLE_DRIVER_HPP_

#include <functional>
#include <cstdint>
#include <unistd.h>

// Do nothing class. Must be used in case if console driver doesn't needed.
// Must have the same interface as platform's console driver has
class console_driver
{
public:
    int init()  { return 0; }
    int open()  { return 0; }
    int close() { return 0; }

    ssize_t write(const uint8_t *data, size_t count)
    { (void) data; (void) count; return 0; }

    ssize_t read(uint8_t *data, size_t count)
    { (void) data; (void) count; return 0; }

    int32_t get_state()
    { return 0; }

    int32_t clear_state(int32_t state)
    { (void) state; return 0; }

    int register_IRQ(const std::function< void() > &handler)
    { (void) handler; return 0; }

    int complete_IRQ(int32_t state)
    { (void) state; return 0; }

    int complete_IRQ()
    { return 0; }

    int deregisterIRQ()
    { return 0; }
};


#endif // DEFAULT_CONSOLE_DRIVER_HPP_
