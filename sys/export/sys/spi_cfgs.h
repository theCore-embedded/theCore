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

// SPI communication type
enum class SPI_com_type
{
    poll,
    IRQ,
    DMA,		// Implies IRQ for DMA events
    DMA_no_IRQ	// DMA without IRQ
};

// Provides general UART state bits
// TODO: extend
// TODO: substitude constexpr to something better
namespace SPI_status
{
constexpr int32_t INVALID	=	-1;			// Invalid status
constexpr int32_t EMPTY		=	0;			// No state
constexpr int32_t ERROR		=	1 << 0;		// General error
constexpr int32_t BSY       =	1 << 1;		// Device is busy
constexpr int32_t TX_RDY    =	1 << 1;		// Transmit line is ready
constexpr int32_t RX_PND   	=	1 << 2;		// Receive pending
constexpr int32_t DMA_HT    =	1 << 0;		// DMA half-transfer complete
constexpr int32_t DMA_TC    =	1 << 1;		// DMA transfer complete
}

// Are equal for now
namespace SPI_irq = SPI_status;

#endif
