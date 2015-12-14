// TODO: better name for this module
#ifndef SYS_SPI_CFGS_H
#define SYS_SPI_CFGS_H

#include <os.hpp>

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

protected:
    // Initializes lock on demand
    static void lock_init();

private:
    // TODO: replace it with special OS abstractions
    static void *m_handle;
};

// TODO: replace it with special OS abstractions
template< SPI_device SPIx >
void* SPI_lock< SPIx >::m_handle = nullptr;


template< SPI_device SPIx >
void SPI_lock< SPIx >::lock_init()
{
    // TODO: error check
    if (m_handle == nullptr) {
        m_handle = semaphore_create();
    }
}

template< SPI_device SPIx >
void SPI_lock< SPIx >::lock()
{
    if (m_handle) {
        semaphore_take(m_handle);
    }
}

template< SPI_device SPIx >
void SPI_lock< SPIx >::unlock()
{
    if (m_handle) {
        semaphore_give(m_handle);
    }
}


#endif
