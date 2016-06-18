#ifndef PLATFORM_COMMON_SPI_HPP_
#define PLATFORM_COMMON_SPI_HPP_

#include <ecl/thread/mutex.hpp>

namespace ecl
{

// TODO: drop it
// Represents distinct peripheral devices
enum class spi_device
{
    bus1,
    bus2,
    bus3,
    bus4,
    bus5,
    bus6,
    bus7,
    bus8
};

//! I2S specific parameters
namespace i2s
{

//! Common audio sample frequencies for I2S
enum class audio_frequency
{
    k192,
    k96,
    k88,
    k48,
    k44,
    k32,
    k22,
    k16,
    k11,
    k8
};

//! Common sample sizes for I2S
enum class data_format
{
    b8,
    b16,
    b24,
    b32
};

} // namespace i2s



} // namespace ecl

#endif // PLATFORM_COMMON_SPI_HPP_
