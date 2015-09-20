#ifndef TARGET_SPI_HPP_
#define TARGET_SPI_HPP_

#include <stm32f4xx.h>
#include <platform/spi_device.hpp>

// TODO: better name
using SpiLCDDriver = SpiDev< SpiDevices::BUS_2 >;

#endif // TARGET_SPI_HPP_

