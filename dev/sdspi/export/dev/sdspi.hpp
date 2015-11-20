#ifndef DEV_SDSPI_HPP
#define DEV_SDSPI_HPP

#include <algorithm>
#include <target/usart.hpp>
#include <ecl/iostream.hpp>

// TODO: mention about 8 additional clocks before each command!!!
template< class SPI_dev, class GPIO_CS >
class SD_SPI
{
public:
    static constexpr size_t block_len = 512;

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
        R1()      :response{0} {}

        uint8_t    response;
    };

    struct R3
    {
        R3()      :r1{}, OCR{0} {}

        R1         r1;
        uint8_t    OCR[4];
    };

    struct R1_read
    {
        R1_read()  :r1{}, token{0}, data{nullptr}, size{0}, crc{0} {}

        R1          r1;
        uint8_t     token; // Either error token or data token
        uint8_t     *data; // Buffer for data
        size_t      size;  // Size of data
        uint16_t    crc;   // Data CRC
    };

    struct R1_CID
    {
        R1_CID()  :r1{}, CID{0}, crc{0} {}
        R1         r1;
        uint8_t    CID[16];
        uint16_t   crc;   // Data CRC
    };

    struct R1_write
    {
        // TODO
    };

    using R7 = R3; // TODO: clarify

    // http://elm-chan.org/docs/mmc/mmc_e.html
    // SD SPI command set ------------------------------------------------------

    // GO_IDLE_STATE            - Software reset.
    int CMD0(R1 &r);

    // SEND_OP_COND             - Initiate initialization process.
    int CMD1(R1 &r);

    // APP_SEND_OP_COND         - For only SDC. Initiate initialization process.
    int ACMD41(R1 &r, bool HCS);

    // SEND_IF_COND             - For only SDC V2. Check voltage range.
    int CMD8(R7 &r);

    // SEND_CSD                 - Read CSD register.
    int CMD9(R1 &r, uint8_t *buf, size_t size);

    // SEND_CID                 - Read CID register.
    int CMD10(R1_read &r);

    // STOP_TRANSMISSION        - Stop to read data.
    // int CMD12(R1 &r); R1b is required here

    // SET_BLOCKLEN             - Change R/W block size.
    int CMD16(R1 &r);

    // READ_SINGLE_BLOCK        - Read a block.
    int CMD17(R1_read &r, uint32_t address);

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

    // Sends CMD
    template< typename R >
    int send_CMD(R &resp, uint8_t CMD_idx, const uint8_t (&argument)[4], uint8_t crc = 0);

    // Sends >= 47 empty clocks to initialize the device
    // int send_init();

    int software_reset();
    int check_conditions();
    int init_process();
    int check_OCR();
    int obtain_card_info();
    int set_block_length();

    ssize_t read_data(uint8_t *buf, size_t size);

    // Useful abstractions
    int receive_response(R1 &r);
    int receive_response(R3 &r);
    int receive_response(R1_read &r);
   // int receive_response(R1_CID &r);

    // Transport layer
    ssize_t send(const uint8_t *buf, size_t size);
    ssize_t receive(uint8_t *buf, size_t size);

    SPI_dev  m_spi;     // SPI device
    bool     m_inited;  // Inited flags
    bool     m_HC;      // High Capacity flag
    int      m_opened;  // Opened times counter
    off_t    m_offset;  // Current offset
};

template< class SPI_dev, class GPIO_CS >
SD_SPI< SPI_dev, GPIO_CS >::SD_SPI()
    :m_spi{}
    ,m_inited{false}
    ,m_HC{false}
    ,m_opened{0}
    ,m_offset{0}
{
}

template< class SPI_dev, class GPIO_CS >
SD_SPI< SPI_dev, GPIO_CS >::~SD_SPI()
{
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::init()
{
    if (!m_inited) {
        m_spi.init();

        m_spi.lock();
        GPIO_CS::set();

        m_spi.unlock();

        m_inited = true;
    }


    return 0;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::open()
{
    if (!m_inited) {
        return -1;
    }

    if (!m_opened) {
        m_spi.open();
        m_spi.lock();

        uint8_t buf[80];
        std::fill_n(buf, sizeof(buf), 0xff);
        send(buf, sizeof(buf));

        if (software_reset() < 0) {
            ecl::cout << "Failed to reset a card" << ecl::endl;
            m_spi.unlock();
            return -1;
        }

        if (check_conditions() < 0) {
            ecl::cout << "Failed to check conditions" << ecl::endl;
            m_spi.unlock();
            return -2;
        }

        if (init_process() < 0) {
            ecl::cout << "Failed to init a card" << ecl::endl;
            m_spi.unlock();
            return -3;
        }


        int ret = check_OCR();
        if (ret < 0) {
            ecl::cout << "Failed to check OCR" << ecl::endl;
            m_spi.unlock();
            return -4;
        // TODO: magic numbers
        } else if (ret == 1) {
            m_HC = true;
        // TODO: magic numbers
        } else if (ret == 2) {
            if (set_block_length() < 0) {
                ecl::cout << "Can't set block length" << ecl::endl;
                return -4;
            }
        } else {
            ecl::cout << "Unsupported type of card" << ecl::endl;
            m_spi.unlock();
            return -5;
        }

        ecl::cout << "Card initialized successfully" << ecl::endl;

        if (obtain_card_info() < 0) {
            ecl::cout << "Failed to obtain card info" << ecl::endl;
            m_spi.unlock();
            return -6;
        }

        m_spi.unlock();
    }

    m_opened++;
    return 0;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::close()
{
    if (!--m_opened) {
        // TODO
    }

    return 0;
}

template< class SPI_dev, class GPIO_CS >
ssize_t SD_SPI< SPI_dev, GPIO_CS >::write(const uint8_t *data, size_t count)
{
    if (!m_opened) {
        return -1;
    }

    return -1;
}

template< class SPI_dev, class GPIO_CS >
ssize_t SD_SPI< SPI_dev, GPIO_CS >::read(uint8_t *data, size_t count)
{
    if (!m_opened) {
        return -1;
    }

    m_spi.lock();
    int ret = read_data(data, count);
    m_spi.unlock();

    return ret;
}


template< class SPI_dev, class GPIO_CS >
off_t SD_SPI< SPI_dev, GPIO_CS >::seek(off_t offset)
{
    if (!m_opened) {
        return -1;
    }

    return -1;
}


template< class SPI_dev, class GPIO_CS >
off_t SD_SPI< SPI_dev, GPIO_CS >::tell() const
{
    if (!m_opened) {
        return -1;
    }

    return -1;
}

// Private methods -------------------------------------------------------------

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::send(const uint8_t *buf, size_t size)
{
    volatile bool completed = false;

    auto handler = [&completed]() {
        completed = true;
    };

    typename SPI_dev::XFER_t spi_xfer;
    spi_xfer.set_buffers(buf, nullptr, size);
    spi_xfer.set_handler(handler);
    m_spi.xfer(spi_xfer);

    while (!completed) { }
    while ((m_spi.get_status() & SPI_dev::flags::BSY)) { }

    // TODO: verify that all data was transfered
    return size;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::receive(uint8_t *buf, size_t size)
{
    volatile bool completed = false;

    auto handler = [&completed]() {
        completed = true;
    };

    typename SPI_dev::XFER_t spi_xfer;
    spi_xfer.set_buffers(nullptr, buf, size);
    spi_xfer.set_handler(handler);
    m_spi.xfer(spi_xfer);

    while (!completed) { }
    while ((m_spi.get_status() & SPI_dev::flags::BSY)) { }

    // TODO: verify that all data was transfered
    return size;
}

template< class SPI_dev, class GPIO_CS >
template< typename R >
int SD_SPI< SPI_dev, GPIO_CS >::send_CMD(R &resp, uint8_t CMD_idx, const uint8_t (&argument)[4], uint8_t crc)
{
    CMD_idx &= 0x3f; // First two bits are reserved TODO: comment
    CMD_idx |= 0x40;

    crc |= 0x1; // EOT flag

    // Comand body
    const uint8_t to_send[] =
    { CMD_idx, argument[0], argument[1], argument[2], argument[3], crc };

    // Send HCS
    send(to_send, sizeof(to_send));

    // Rerieve a result
    receive_response(resp);

    // Complete transaction
    const uint8_t dummy = 0xff;
    send(&dummy, 1);

    return 0;
}

//------------------------------------------------------------------------------

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::receive_response(R1 &r)
{
    uint8_t tries = 8;
    do {
        receive(&r.response, 1);
    } while (r.response == 0xff && --tries);

    return tries ? 0 : -1;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::receive_response(R3 &r)
{
    if (receive_response(r.r1) < 0)
        return -1;

    receive((uint8_t *)r.OCR, sizeof(r.OCR));

    return 0;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::receive_response(R1_read &r)
{
    if (receive_response(r.r1) < 0)
        return -1;

    if (r.r1.response & ~1)
        return -1;

    uint8_t tries = 64;

    do {
        if (receive(&r.token, sizeof(r.token)) < 0)
            return -2;
    } while (r.token == 0xff && --tries);

    if (r.token == 0xfe)
        if (receive(r.data, r.size) < 0)
            return -3;

    if (receive((uint8_t *)&r.crc, sizeof(r.crc)) < 0)
        return -4;

    return 0;
}

//------------------------------------------------------------------------------

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::CMD0(R1 &r)
{
    GPIO_CS::reset();

    // TODO: comments
    constexpr uint8_t CMD0_idx = 0;
    constexpr uint8_t CMD0_crc = 0x95;
    constexpr uint8_t arg[4]   = {0};

    send_CMD(r, CMD0_idx, arg, CMD0_crc);
    GPIO_CS::set();

    return 0;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::CMD8(R7 &r)
{
    GPIO_CS::reset();

    // TODO: comments
    constexpr uint8_t CMD8_idx = 8;
    constexpr uint8_t CMD8_crc = 0x87;
    constexpr uint8_t arg[]    = { 0, 0, 0x01, 0xaa };

    send_CMD(r, CMD8_idx, arg, CMD8_crc);

    GPIO_CS::set();

    return 0;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::CMD10(R1_read &r)
{
    GPIO_CS::reset();

    // TODO: comments
    constexpr uint8_t CMD10_idx  = 10;
    constexpr uint8_t arg[4]     = { 0 };

    send_CMD(r, CMD10_idx, arg);

    GPIO_CS::set();

    return 0;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::CMD16(R1 &r)
{
    GPIO_CS::reset();

    // TODO: comments
    constexpr uint8_t CMD16_idx = 16;
    constexpr uint8_t arg[4]    = {
        (uint8_t) (block_len >> 24),
        (uint8_t) (block_len >> 16),
        (uint8_t) (block_len >> 8),
        (uint8_t) (block_len),
    };

    send_CMD(r, CMD16_idx, arg);

    GPIO_CS::set();

    return 0;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::CMD17(R1_read &r, uint32_t address)
{
    GPIO_CS::reset();

    // TODO: comments
    constexpr uint8_t CMD17_idx = 17;
    const uint8_t arg[] = {
        (uint8_t) (address >> 24),
        (uint8_t) (address >> 16),
        (uint8_t) (address >> 8),
        (uint8_t) (address),
    };

    send_CMD(r, CMD17_idx, arg);

    GPIO_CS::set();

    return 0;
}


template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::CMD55(R1 &r)
{
    GPIO_CS::reset();

    // TODO: comments
    constexpr uint8_t CMD55_idx = 55;
    constexpr uint8_t arg[]     = { 0, 0, 0, 0 };

    send_CMD(r, CMD55_idx, arg);

    GPIO_CS::set();

    return 0;
}



template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::CMD58(R3 &r)
{
    GPIO_CS::reset();

    // TODO: comments
    constexpr uint8_t CMD58_idx = 58;
    constexpr uint8_t arg[]     = { 0, 0, 0, 0 };

    send_CMD(r, CMD58_idx, arg);

    GPIO_CS::set();


    return 0;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::ACMD41(R1 &r, bool HCS)
{
    const uint8_t HCS_byte = HCS ? (1 << 6) : 0;
    CMD55(r);

    // Any bit is error except idle bit
    if (r.response & 0xfe) {
        return 0;
    }

    // TODO: comments
    constexpr uint8_t  CMD41_idx  = 41;
    const uint8_t      arg[]      = { HCS_byte, 0, 0, 0 };

    GPIO_CS::reset();
    send_CMD(r, CMD41_idx, arg);
    GPIO_CS::set();

    return 0;
}

//------------------------------------------------------------------------------

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::software_reset()
{
    R1 r1;

    CMD0(r1);

    if (r1.response == 0x1) {
        ecl::cout << "Card is in IDLE state, as expected" << ecl::endl;
        return 0;
    }

    return -1;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::check_conditions()
{
    R7 r7;

    CMD8(r7);

    if (r7.r1.response == 0x1) {
        if (r7.OCR[3] != 0xAA) {
            ecl::cout << "Check pattern mismatch" << ecl::endl;
            return -1;
        }

        if (!r7.OCR[2]) {
            ecl::cout << "Voltage range not accepted" << ecl::endl;
            return -2;
        }

        return 0;
    }

    return -1;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::init_process()
{
    //TODO: comments
    R1 r1;

    r1.response = 0x1;
    uint8_t tries = 255;

    while (r1.response == 0x1 && --tries) {
        ACMD41(r1, true);

        if (r1.response & 0xfe) {
            return -1;
        }
    }

    if (!tries) {
        return -2;
    }

    return 0;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::check_OCR()
{
    R3 r3;
    CMD58(r3);
    int ret = 0;

    // TODO: compare current profile and supported one!
    // Otherwise we can damage the card
    if (!r3.r1.response) {
        // Card sends data (as well as OCR) in BE format
        if (r3.OCR[0] & (1 << 7)) {
            ecl::cout << "Card is powered-up" << ecl::endl;
            if (r3.OCR[0] & (1 << 6)) {
                ecl::cout << "Card is High capacity" << ecl::endl;
                ret = 1;
            } else {
                ecl::cout << "Card is Standard capacity" << ecl::endl;
                ret = 2;
            }
        }

        // Whole byte 2 of OCR contains voltage profiles
        for (uint8_t bit = 0; bit < 8; ++bit) {
            if (r3.OCR[2] & (1 << bit)) {
                uint8_t V = bit + 27;
                uint intgr = V / 10;
                uint fract = V - intgr * 10;
                ecl::cout << "VOLTAGE: ["
                          << intgr << '.' << fract
                          << "; +0.1]" << ecl::endl;
            }
        }

        // Last bit in byte 3 of OCR contains one voltage profile
        if (r3.OCR[3] & (1 << 8)) {
            ecl::cout << "VOLTAGE: [2.7,+0.1]" << ecl::endl;
        }

        return ret;
    }

    return -1;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::obtain_card_info()
{
    R1_read r1_read;
    uint8_t cid[16];
    r1_read.data = cid;
    r1_read.size = sizeof(cid);

    CMD10(r1_read);

    if (r1_read.token == 0xfe) {
        ecl::cout << "------------------------------------" << ecl::endl;
        ecl::cout << "Manufacturer ID:       " << cid[0] << ecl::endl;
        ecl::cout << "OEM/Application ID:    " << (char) cid[1]
                  << (char) cid[2] << ecl::endl;

        ecl::cout << "Product name:          ";
        for (uint i = 3; i < 8; i++)
            ecl::cout << (char) cid[i];
        ecl::cout << ecl::endl;
        ecl::cout << "Product revision:      ";
        ecl::cout << (cid[8] >> 4) << '.' << (cid[8] & 0xf) << ecl::endl;
        ecl::cout << "S/N:                   "
                  << ((cid[9] << 24) | (cid[10] << 16)
                | (cid[11] << 8) | cid[12]) << ecl::endl;

        uint8_t month = cid[1] & 0xf;
        uint16_t year = 2000 + ((cid[1] >> 4) | (cid[2] & 0xf));

        ecl::cout << "Date:                  " << month << '.' << year << ecl::endl;
        ecl::cout << "------------------------------------" << ecl::endl;
        return 0;
    }

    return -1;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::set_block_length()
{
    R1 r1;

    CMD16(r1);

    if (r1.response != 0) {
        ecl::cout << "Can't get valid response of CMD16: " << r1.response << ecl::endl;
        return -1;
    }

    return 0;
}


// Get rid of this hack
extern "C" {
void *memcpy(void *dst, const void *src, size_t cnt);
}

template< class SPI_dev, class GPIO_CS >
ssize_t SD_SPI< SPI_dev, GPIO_CS >::read_data(uint8_t *buf, size_t size)
{
    // TODO: optimize it
    // TODO: modify it with respect to HC\SC type
    uint8_t fallback[512];
    R1_read r1_read;

    r1_read.size = 512;
    r1_read.data = fallback;

    CMD17(r1_read, m_offset);

    if (r1_read.r1.response != 0) {
        ecl::cout << "Failed to get R1: " << (int) r1_read.r1.response << ecl::endl;
        return -1;
    }

    if (r1_read.token != 0xfe) {
        ecl::cout << "Data retrieval fails: " << (int) r1_read.token << ecl::endl;
        return -2;
    }

    size_t to_copy = std::min(size, r1_read.size);
    // UND ref. to memmove TODO
    //std::copy(r1_read.data, r1_read.data + to_copy, buf);

    memcpy(buf, r1_read.data, to_copy);
    m_offset += to_copy;
    return to_copy;
}

#endif // DEV_SDSPI_H
