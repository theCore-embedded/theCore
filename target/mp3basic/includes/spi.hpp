#ifndef TARGET_SPI_HPP_
#define TARGET_SPI_HPP_

#include <SpiDev.hpp>
#include <stm32f4xx.h>

// TODO: better name
using SpiLCDDriver = SpiDev< (std::uintptr_t) SPI2 >;

#endif // TARGET_SPI_HPP_

