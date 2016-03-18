#ifndef PLATFORM_COMMON_SPI_HPP_
#define PLATFORM_COMMON_SPI_HPP_

#include <ecl/thread/mutex.hpp>

namespace ecl
{

// TODO: drop it
// Represents distinct peripheral devices
enum class spi_device
{
    bus_1,
    bus_2,
    bus_3,
    bus_4,
    bus_5,
    bus_6,
    bus_7,
    bus_8
};

}

#endif // PLATFORM_COMMON_SPI_HPP_
