#ifndef PLATFORM_SPI_DEV_H_
#define PLATFORM_SPI_DEV_H_

#include <stm32f4xx_spi.h>

template< std::uintptr_t SPI_InitTypeDef >
class SpiDev
{
public:
	SpiDev();
	~SpiDev();

	// Lazy initialization, -1 if error. 0 otherwise
	int init();

	// -1 if error, 0 otherwise
	int open();
	// -1 if error, 0 otherwise
	int close();

	// -1 if error, [0, count] otherwise
	ssize_t write(const uint8_t *data, size_t count);

	// -1 if error, [0, count] otherwise
	ssize_t read(uint8_t *data, size_t count);

private:
	// Indicates that SPI is ready for use
	int m_inited = 0;
	// Flag to prevent multiple opening
	int m_opened = 0;
};

#endif // PLATFORM_SPI_DEV_H_
