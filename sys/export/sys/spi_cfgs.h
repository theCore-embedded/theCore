// TODO: better name for this module
#ifndef SYS_SPI_CFGS_H
#define SYS_SPI_CFGS_H

// Represents distinct peripheral devices
enum class SPI_device
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

enum class SPI_direction
{
    TX,
    RX,
    BIDIR
};

enum class SPI_mode
{
    master,
    slave
};

// Clock polarity
enum class SPI_CPOL
{
    low,
    high
};

// Clock phase
enum class SPI_CPHA
{
    first_edge,
    second_edge
};

enum class SPI_NSS_type
{
    HW,
    SW
};

enum class SPI_bit_order
{
    MSB,
    LSB
};


enum class SPI_io
{
    poll,
    IRQ,
    DMA,		// Implies IRQ for DMA events
    DMA_no_IRQ	// DMA without IRQ
};


#endif
