#ifndef PLATFORM_SPI_DEV_H_
#define PLATFORM_SPI_DEV_H_

#include <stm32f4xx_spi.h>
#include <spi_cfgs.h>


// TODO: replace SPIx with generic enumerator
template< std::uintptr_t SPIx >
class SpiDev
{
public:
	// TODO: default values
	SpiDev(SpiDirection   direction = SpiDirection::BIDIR,
		   SpiMode        mode      = SpiMode::MASTER,
		   SpiCPOL        CPOL      = SpiCPOL::LOW,
		   SpiCPHA        CPHA      = SpiCPHA::SECOND_EDGE,
		   SpiNssType     nssType   = SpiNssType::HW,
		   SpiBitOrder    bitOrder  = SpiBitOrder::MSB,
		   uint32_t       clock     = 0); // Not used for now. TODO here
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
	static constexpr auto pickSPI();
	static constexpr auto pickRCC();
	static constexpr auto pickRCC_fn();

	static constexpr auto pickDir(SpiDirection direction);
	static constexpr auto pickMode(SpiMode mode);
	static constexpr auto pickCPOL(SpiCPOL CPOL);
	static constexpr auto pickCPHA(SpiCPHA CPHA);
	static constexpr auto pickNSS(SpiNssType nssType);
	static constexpr auto pickFirstBit(SpiBitOrder bitOrder);

	// Init object is required to be non-constant
	// See SPI_Init routine for more info
	SPI_InitTypeDef m_initObj;
	// TODO: use it!
	const uint32_t        m_clock;

	// TODO: flags instead of two separate fields
	// Indicates that SPI is ready for use
	bool m_inited = false;
	// Flag to prevent multiple opening
	bool m_opened = false;
};

//------------------------------------------------------------------------------
// Interface

template< std::uintptr_t SPIx >
SpiDev< SPIx >::SpiDev(SpiDirection   direction,
					   SpiMode        mode,
					   SpiCPOL        CPOL,
					   SpiCPHA        CPHA,
					   SpiNssType     nssType,
					   SpiBitOrder    bitOrder,
					   uint32_t       clock)
	:m_initObj{
		 pickDir(direction),
		 pickMode(mode),
		 SPI_DataSize_8b, // TODO: carefully configure
		 pickCPOL(CPOL),
		 pickCPHA(CPHA),
		 pickNSS(nssType),
		 SPI_BaudRatePrescaler_2, // TODO: clarify
		 pickFirstBit(bitOrder),
		 7,
		 }
	,m_clock(clock)

{
}

template< std::uintptr_t SPIx >
SpiDev< SPIx >::~SpiDev()
{
	// TODO
	if (m_inited) {

	}
}


// TODO: implement, comments
template< std::uintptr_t SPIx >
int SpiDev< SPIx >::init()
{
	constexpr auto SPIbus = pickSPI();
	auto RCC_Periph = pickRCC();
	auto RCC_fn = pickRCC_fn();

	RCC_fn(RCC_Periph, ENABLE);

	m_inited = true;

	SPI_Init(SPIbus, &m_initObj);

	return -1;
}

// TODO: implement, comments
template< std::uintptr_t SPIx >
int SpiDev< SPIx >::open()
{
	if (!m_inited)
		return -1;

	SPI_Cmd(pickSPI(), ENABLE);
	m_opened = true;
	return 0;
}

// TODO: implement, comments
template< std::uintptr_t SPIx >
int SpiDev< SPIx >::close()
{
	if (!m_opened)
		return -1;

	SPI_Cmd(pickSPI(), DISABLE);
	m_opened = false;
	return 0;
}

// TODO: implement, comments
template< std::uintptr_t SPIx >
ssize_t SpiDev< SPIx >::write(const uint8_t *data, size_t count)
{
	if (!m_opened)
		return -1;

	if (!count)
		return 0;

	constexpr auto SPIbus = pickSPI();

	while (SPI_I2S_GetFlagStatus(SPIbus, SPI_I2S_FLAG_TXE) == RESET) { }

	SPI_I2S_SendData(SPIbus, data[0]);

	return 1;
}

// TODO: implement, comments
template< std::uintptr_t SPIx >
ssize_t SpiDev< SPIx >::read(uint8_t *data, size_t count)
{
	if (!m_opened)
		return -1;

	if (!count)
		return 0;

	constexpr auto SPIbus = pickSPI();

	while (SPI_I2S_GetFlagStatus(SPIbus, SPI_I2S_FLAG_RXNE) == RESET) { }

	data[0] = SPI_I2S_ReceiveData(I2S3ext);

	return 1;
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

// TODO: implement
template< std::uintptr_t SPIx >
constexpr auto SpiDev< SPIx >::pickDir(SpiDirection direction)
{

	// TODO: add default section
	switch (direction) {
	case SpiDirection::TX:
		return SPI_Direction_1Line_Tx;
	case SpiDirection::RX:
		return SPI_Direction_1Line_Rx;
	case SpiDirection::BIDIR:
		return SPI_Direction_2Lines_FullDuplex;
	default:
		return (uint16_t) -1;
	}
}

// TODO: implement
template< std::uintptr_t SPIx >
constexpr auto SpiDev< SPIx >::pickMode(SpiMode mode)
{
	switch (mode) {
	case SpiMode::MASTER:
		return SPI_Mode_Master;
	case SpiMode::SLAVE:
		return SPI_Mode_Slave;
	default:
		return (uint16_t) -1;
	}
}

// TODO: implement
template< std::uintptr_t SPIx >
constexpr auto SpiDev< SPIx >::pickCPOL(SpiCPOL CPOL)
{
	switch (CPOL) {
	case SpiCPOL::LOW:
		return SPI_CPOL_Low;
	case SpiCPOL::HIGH:
		return SPI_CPOL_High;
	default:
		return (uint16_t) -1;
	}
}

// TODO: implement
template< std::uintptr_t SPIx >
constexpr auto SpiDev< SPIx >::pickCPHA(SpiCPHA CPHA)
{
	switch (CPHA) {
	case SpiCPHA::FIRST_EDGE:
		return SPI_CPHA_1Edge;
	case SpiCPHA::SECOND_EDGE:
		return SPI_CPHA_2Edge;
	default:
		// TODO: clarify
		return (uint16_t) -1;
	}
}

template< std::uintptr_t SPIx >
constexpr auto SpiDev< SPIx >::pickNSS(SpiNssType nssType)
{
	switch (nssType) {
	case SpiNssType::HW:
		return SPI_NSS_Hard;
	case SpiNssType::SW:
		return SPI_NSS_Soft;
	default:
		// TODO: clarify
		return (uint16_t) -1;
	}
}

template< std::uintptr_t SPIx >
constexpr auto SpiDev< SPIx >::pickFirstBit(SpiBitOrder bitOrder)
{
	switch (bitOrder) {
	case SpiBitOrder::MSB:
		return SPI_FirstBit_MSB;
	case SpiBitOrder::LSB:
		return SPI_FirstBit_LSB;
	default:
		// TODO: clarify
		return (uint16_t) -1;
	}

	return SPI_FirstBit_MSB;
}



#endif // PLATFORM_SPI_DEV_H_
