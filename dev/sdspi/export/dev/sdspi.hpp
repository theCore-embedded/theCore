#ifndef DEV_SDSPI_HPP
#define DEV_SDSPI_HPP

#include <algorithm>
#include <target/usart.hpp>

template< class SPI_dev, class GPIO_CS >
class SD_SPI
{
public:
    SD_SPI();
    ~SD_SPI();

    // Lazy initialization, -1 if error. 0 otherwise
    int init();

    // -1 if error, 0 otherwise
    int open();
    // -1 if error, 0 otherwise
    int close();

    // < count if error, count otherwise
    ssize_t write(const uint8_t *data, size_t count);
    // -1 if error, [0, count] otherwise
    ssize_t read(uint8_t *data, size_t count);

    // TODO: extend it with SEEK_CUR and SEEK_END
    off_t seek(off_t offset /*, SEEK_SET */);

    // Tell current position
    // -1 if error, valid offset otherwise
    off_t tell() const;

private:
    using DMA_rx = typename SPI_dev::DMA_RX_t;
    using DMA_tx = typename SPI_dev::DMA_TX_t;

    struct R1
    {
        uint8_t     response;
    };

    struct R3
    {
        R1          r1;
        uint32_t    OCR;
    };

    using R7 = R3; // TODO: clarify

    // http://elm-chan.org/docs/mmc/mmc_e.html
    // SD SPI command set ------------------------------------------------------

    // GO_IDLE_STATE            - Software reset.
    int CMD0(R1 &r);

    // SEND_OP_COND             - Initiate initialization process.
    int CMD1(R1 &r);

    // APP_SEND_OP_COND         - For only SDC. Initiate initialization process.
    int ACMD41(R1 &r, uint32_t argument);

    // SEND_IF_COND             - For only SDC V2. Check voltage range.
    int CMD8(R7 &r, uint32_t argument);

    // SEND_CSD                 - Read CSD register.
    int CMD9(R1 &r, uint8_t *buf, size_t size);

    // SEND_CID                 - Read CID register.
    int CMD10(R1 &r, uint8_t *buf, size_t size);

    // STOP_TRANSMISSION        - Stop to read data.
    // int CMD12(R1 &r); R1b is required here

    // SET_BLOCKLEN             - Change R/W block size.
    int CMD16(R1 &r, uint32_t block_length);

    // READ_SINGLE_BLOCK        - Read a block.
    int CMD17(R1 &r, uint32_t address, uint8_t *buf, size_t size);

    // READ_MULTIPLE_BLOCK      - Read multiple blocks.
    int CMD18(R1 &r, uint32_t address, uint8_t *buf, size_t size);

    // SET_BLOCK_COUNT          - For only MMC. Define number of blocks to transfer
    //                            with next multi-block read/write command.
    int CMD23(R1 &r, uint16_t block_count);

    // SET_WR_BLOCK_ERASE_COUNT - For only SDC. Define number of blocks to pre-erase
    //                              with next multi-block write command.
    int ACMD23(R1 &r, uint16_t block_count);

    // WRITE_BLOCK              - Write a block.
    int CMD24(R1 &r, uint32_t address, uint8_t *buf, size_t size);

    // WRITE_MULTIPLE_BLOCK     - Write multiple blocks.
    int CMD25(R1 &r, uint32_t address, uint8_t *buf, size_t size);

    // APP_CMD                  - Leading command of ACMD<n> command.
    int CMD55(R1 &r);

    // READ_OCR                 - Read OCR.
    int CMD58(R3 &r);

    // Sends >= 47 empty clocks to initialize the device
    // int send_init();

    // Useful abstractions
    int receive_R1(R1 &r);
    int receive_R3(R3 &r);

    ssize_t send(const uint8_t *buf, size_t size);
    ssize_t receive(uint8_t *buf, size_t size);


    SPI_dev m_spi;
};

template< class SPI_dev, class GPIO_CS >
SD_SPI< SPI_dev, GPIO_CS >::SD_SPI()
{
}

template< class SPI_dev, class GPIO_CS >
SD_SPI< SPI_dev, GPIO_CS >::~SD_SPI()
{
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::init()
{
    m_spi.init();

    m_spi.lock();
    GPIO_CS::set();

    m_spi.unlock();

    return 0;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::open()
{
    m_spi.open();
    m_spi.lock();

    // TODO: place >= 47 clocks in init()
    uint8_t buf[100];
    std::fill_n(buf, sizeof(buf), 0xff);

    send(buf, sizeof(buf));

    R1 response;

    GPIO_CS::reset();
    CMD0(response);

    if (response.response == 0xff) {
        write_string("Response wasn't received\r\n");
    } else if (response.response == 0x1) {
        write_string("Response OK\r\n");
    } else {
        write_string("Response received but indicates error\n");
    }

    GPIO_CS::set();

    m_spi.unlock();

    return 0;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::close()
{
    return -1;
}

template< class SPI_dev, class GPIO_CS >
ssize_t SD_SPI< SPI_dev, GPIO_CS >::write(const uint8_t *data, size_t count)
{
    return -1;
}

template< class SPI_dev, class GPIO_CS >
ssize_t SD_SPI< SPI_dev, GPIO_CS >::read(uint8_t *data, size_t count)
{
    return -1;
}


template< class SPI_dev, class GPIO_CS >
off_t SD_SPI< SPI_dev, GPIO_CS >::seek(off_t offset)
{
    return -1;
}


template< class SPI_dev, class GPIO_CS >
off_t SD_SPI< SPI_dev, GPIO_CS >::tell() const
{
    return -1;
}

// Private methods -------------------------------------------------------------

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::send(const uint8_t *buf, size_t size)
{
    bool completed = false;

    auto handler = [&completed]() {
        completed = true;
    };

    DMA_tx dma;
    dma.enable_IRQ(handler, DMA_tx::flags::TC);
    dma.set_origin(DMA_tx::role::memory, buf, size);

    m_spi.write(dma);

    while (!completed) { }
    while ((m_spi.get_status() & SPI_dev::flags::BSY)) { }

    dma.complete();

    // TODO: verify that all data was transfered
    return size;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::receive(uint8_t *buf, size_t size)
{
    bool completed = false;

    auto handler = [&completed]() {
        completed = true;
    };

    DMA_rx dma;
    dma.enable_IRQ(handler, DMA_rx::flags::TC);
    dma.set_destination(DMA_rx::role::memory, buf, size);

    m_spi.read(dma);

    while (!completed) { }
    while ((m_spi.get_status() & SPI_dev::flags::BSY)) { }

    dma.complete();

    // TODO: verify that all data was transfered
    return size;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::receive_R1(R1 &r)
{
    uint8_t tries = 8;
    do {
        receive(&r.response, 1);
    } while (r.response == 0xff && --tries);

    return 0;
}


template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::CMD0(R1 &r)
{
    (void) r;
    constexpr uint8_t CMD0[] = { (1 << 6), 0, 0, 0, 0, 0x95 };

    // Ask for initialization
    send(CMD0, sizeof(CMD0));
    // Rerieve a result
    receive_R1(r);

    return 0;
}

#endif // DEV_SDSPI_H
