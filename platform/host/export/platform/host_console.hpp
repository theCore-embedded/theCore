#ifndef HOST_CONSOLE_HPP_
#define HOST_CONSOLE_HPP_

#include <iostream>

class host_console
{
public:
    int init() { return 0; }
    int open() { return 0; }
    int close() { return 0; }

    // -1 if error, [0, count] otherwise
    ssize_t write(const uint8_t *data, size_t count)
    {
        size_t to_write = count;
        while (to_write--) { std::putchar(*data++); }
        return count;
    }

    // -1 if error, [0, count] otherwise
    ssize_t read(uint8_t *data, size_t count)
    {
        size_t to_write = count;
        while(to_write--) { *data++ = std::getchar(); }
        return count;
    }
};

#endif // HOST_CONSOLE_HPP_
