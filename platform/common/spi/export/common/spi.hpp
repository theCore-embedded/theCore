#ifndef COMMON_SPI_HPP
#define COMMON_SPI_HPP

#include <ecl/thread/mutex.hpp>

// TODO: drop it
// Represents distinct peripheral devices
enum class spi_device
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

// TODO: drop it when SPI will be migrated
using SPI_device = spi_device;


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

// TODO: move to better place
// SPI locking base class, provides a mutex for every bus in the system
template< SPI_device SPIx >
class SPI_lock
{
public:
    // Locks a bus
    static void lock();
    // Unlocks a bus
    static void unlock();

private:
    // TODO: replace it with special OS abstractions
    static ecl::mutex m_lock;
};

// TODO: replace it with special OS abstractions
template< SPI_device SPIx >
ecl::mutex SPI_lock< SPIx >::m_lock;

template< SPI_device SPIx >
void SPI_lock< SPIx >::lock()
{
    m_lock.lock();
}

template< SPI_device SPIx >
void SPI_lock< SPIx >::unlock()
{
    m_lock.unlock();
}


#endif // COMMON_SPI_HPP
