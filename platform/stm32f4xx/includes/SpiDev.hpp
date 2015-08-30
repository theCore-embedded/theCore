#ifndef PLATFORM_SPI_DEV_H_
#define PLATFORM_SPI_DEV_H_

#include <stm32f4xx_spi.h>
#include <spi_cfgs.h>


// TODO: think about passing parameters trough in run time
// rather in compile time
template< std::uintptr_t SPIx >
class SpiDev
{
public:
	SpiDev(SpiDirection   direction,
		   SpiMode        mode,
		   SpiCPOL        CPOL,
		   SpiCPHA        CPHA,
		   SpiNssType     nssType,
		   SpiBitOrder    bitOrder,
		   uint32_t       clock)
		:m_dir(direction)
		,m_mode(mode)
		,m_CPOL(CPOL)
		,m_CPHA(CPHA)
		,m_nssType(nssType)
		,m_bitOrder(bitOrder)
		,m_clock(clock)
	{}
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
	constexpr auto pickSPI();
	constexpr auto pickRCC();
	constexpr auto pickRCC_fn();

	const SpiDirection m_dir;
	const SpiMode      m_mode;
	const SpiCPOL      m_CPOL;
	const SpiCPHA      m_CPHA;
	const SpiNssType   m_nssType;
	const SpiBitOrder  m_bitOrder;
	const uint32_t     m_clock;

	// Indicates that SPI is ready for use
	int m_inited = 0;
	// Flag to prevent multiple opening
	int m_opened = 0;
};

//------------------------------------------------------------------------------
// Interface

// TODO: implement
template< std::uintptr_t SPIx >
int SpiDev< SPIx >::init()
{
	return -1;
}

// TODO: implement
template< std::uintptr_t SPIx >
int SpiDev< SPIx >::open()
{
	return -1;
}

// TODO: implement
template< std::uintptr_t SPIx >
int SpiDev< SPIx >::close()
{
	return -1;
}

// TODO: implement
template< std::uintptr_t SPIx >
ssize_t SpiDev< SPIx >::write(const uint8_t *data, size_t count)
{
	return -1;
}

// TODO: implement
template< std::uintptr_t SPIx >
ssize_t SpiDev< SPIx >::read(uint8_t *data, size_t count)
{
	return -1;
}

//------------------------------------------------------------------------------
// Private section

// TODO: implement
template< std::uintptr_t SPIx >
constexpr auto SpiDev< SPIx >::pickSPI()
{
	return (SPI_TypeDef *) SPIx;
}

// TODO: implement
template< std::uintptr_t SPIx >
constexpr auto SpiDev< SPIx >::pickRCC()
{
	return RCC_APB2Periph_SPI1;
}

// TODO: implement
template< std::uintptr_t SPIx >
constexpr auto SpiDev< SPIx >::pickRCC_fn()
{
	return RCC_APB2PeriphClockCmd;
}


#endif // PLATFORM_SPI_DEV_H_
