#ifndef TARGET_SPI_HPP_
#define TARGET_SPI_HPP_

#include <stm32f4xx.h>
#include <platform/spi_device.hpp>
#include <platform/dma_device.hpp>

using SPI_LCD_config =
SPI_config<
SPI_com_type::DMA,
SPI_Direction_1Line_Tx,
SPI_Mode_Master,
SPI_CPOL_High,
SPI_CPHA_2Edge,
SPI_NSS_Soft,
SPI_FirstBit_MSB,
1000000
>;

// TODO: better name
using SPI_LCD_driver =
SPI_dev<
SPI_device::bus_2,
SPI_LCD_config,
DMA_dev< (std::uintptr_t) DMA1_Stream4, 0 >,
DMA_dev< (std::uintptr_t) DMA1_Stream3, 0 >
>;


#endif // TARGET_SPI_HPP_

