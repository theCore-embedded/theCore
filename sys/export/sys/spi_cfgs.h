// TODO: better name for this module
#ifndef SYS_SPI_CFGS_H
#define SYS_SPI_CFGS_H

// Represents distinct peripheral devices
enum class SpiDevices
{
	BUS_1,
	BUS_2,
	BUS_3,
	BUS_4,
	BUS_5,
	BUS_6,
	BUS_7,
	BUS_8
};

enum class SpiDirection
{
	TX,
	RX,
	BIDIR
};

enum class SpiMode
{
	MASTER,
	SLAVE
};

// Clock polarity
enum class SpiCPOL
{
	LOW,
	HIGH
};

// Clock phase
enum class SpiCPHA
{
	FIRST_EDGE,
	SECOND_EDGE
};

enum class SpiNssType
{
	HW,
	SW
};

enum class SpiBitOrder
{
	MSB,
	LSB
};

#endif
