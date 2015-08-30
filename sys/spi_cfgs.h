// TODO: better name for this module
#ifndef SYS_SPI_CFGS_H
#define SYS_SPI_CFGS_H

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

enum class SpiCPOL
{
	Low,
	High
};

enum class SpiCPHA
{
	FirstEdge,
	SecondEdge
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
