// TODO: better name for this module
#ifndef SYS_SPI_CFGS_H
#define SYS_SPI_CFGS_H

// TODO: drop it
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

// SPI communication type
enum class SPI_com_type
{
    poll,
    IRQ,
    DMA,		// Implies IRQ for DMA events
    DMA_no_IRQ	// DMA without IRQ
};

// Provides general SPI state bits
// Has to be included to the SPI device
struct SPI_flags
{
    const int32_t empty;		// No state
    const int32_t ERR;          // Simple error
    const int32_t BSY;          // Device is busy
    const int32_t TX_RDY;		// Transmit line is ready
    const int32_t RX_PND;		// Receive pending
};

#endif
