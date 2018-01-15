/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef DEV_SDSPI_HPP
#define DEV_SDSPI_HPP

#include <algorithm>
#include <array>
#include <string.h>

#include <ecl/iostream.hpp>
#include <ecl/endian.hpp>
#include <ecl/types.h>

namespace ecl
{

// TODO: mention about 8 additional clocks before each command!!!
template< class spi_dev, class GPIO_CS >
class sd_spi
{
public:
    sd_spi();
    ~sd_spi();

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
    // Flushes buffered data
    int flush();

    // TODO: extend it with SEEK_CUR and SEEK_END
    // Seeks to the given position, in bytes
    int seek(off_t offset /*, SEEK_SET */);

    // Tell current position
    // -1 if error, valid offset otherwise
    off_t tell() const;

    // Returns a length of a block
    constexpr size_t get_block_length();

    // Seek flags
    enum class seekdir
    {
        beg = 0,
        cur = 1,
        end = 2
    };

private:
    // Return types.
    // Enums omitted, since it should be replaced
    // with system-wide error flags
    static constexpr int sd_expired  = -3;  // Event expired
    static constexpr int sd_spi_err  = -2;  // SPI returns error
    static constexpr int sd_err      = -1;  // General error
    static constexpr int sd_ok       = 0;   // Indicates success
    static constexpr int sd_type_hc  = 1;   // The card is high capacity
    static constexpr int sd_type_sc  = 2;   // The card is standart capacity

    // R1 response
    struct R1
    {
        R1()      :response{0} {}
        uint8_t    response;

        bool ok() const
        {
            // If anything set besides idle flags it is an error
            return received() && !(response & ~idle_state);
        }

        bool received() const
        {
            // Reserved bit indicates a start of R1 response
            return !(response & reserved_bit);
        }

        // Flags that can be found in response
        static constexpr uint8_t idle_state    = 0x01;
        static constexpr uint8_t erase_reset   = 0x02;
        static constexpr uint8_t illegal_cmd   = 0x04;
        static constexpr uint8_t com_crc_err   = 0x08;
        static constexpr uint8_t erase_seq_err = 0x10;
        static constexpr uint8_t addr_err      = 0x20;
        static constexpr uint8_t param_error   = 0x40;
        // Must be set to 0 in response
        static constexpr uint8_t reserved_bit  = 0x80;
    };

    // R3 response
    struct R3
    {
        R3()      :r1{}, OCR{0} {}

        R1         r1;
        uint32_t   OCR;
    };

    // Are the same
    using R7 = R3;
    using argument = std::array< uint8_t, 4 >;

    struct block_buffer
    {
        block_buffer() :block{0}, origin{0}, mint{true} { }

        // Block length is given without respect to the card type.
        // If card is Standart Capacity then block length will be set to 512,
        // ( see set_block_length() ).
        // If card is High Capacity then block length is fixed to 512 by design.
        static constexpr size_t block_len = 512;

        uint8_t block[block_len];     // The block itself
        size_t  origin;               // The offset from which block was obtained
        bool    mint;                 // True if there were no writes in buffer
    };

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
    int CMD10(R1 &r);

    // STOP_TRANSMISSION        - Stop to read data.
    // int CMD12(R1 &r); R1b is required here

    // SET_BLOCKLEN             - Change R/W block size.
    int CMD16(R1 &r);

    // READ_SINGLE_BLOCK        - Read a block.
    int CMD17(R1 &r, uint32_t address);

    // READ_MULTIPLE_BLOCK      - Read multiple blocks.
    int CMD18(R1 &r, uint32_t address, uint8_t *buf, size_t size);

    // SET_BLOCK_COUNT          - For only MMC. Define number of blocks to transfer
    //                            with next multi-block read/write command.
    int CMD23(R1 &r, uint16_t block_count);

    // SET_WR_BLOCK_ERASE_COUNT - For only SDC. Define number of blocks to pre-erase
    //                              with next multi-block write command.
    int ACMD23(R1 &r, uint16_t block_count);

    // WRITE_BLOCK              - Write a block.
    int CMD24(R1 &r, uint32_t address);

    // WRITE_MULTIPLE_BLOCK     - Write multiple blocks.
    int CMD25(R1 &r, uint32_t address, uint8_t *buf, size_t size);

    // APP_CMD                  - Leading command of ACMD<n> command.
    int CMD55(R1 &r);

    // READ_OCR                 - Read OCR.
    int CMD58(R3 &r);

    // Sends CMD
    template< typename R >
    int send_CMD(R &resp, uint8_t CMD_idx, const argument &arg, uint8_t crc = 0);

    // Sends >= 47 empty clocks to initialize the device
    int send_init();
    int open_card();
    int software_reset();
    int check_conditions();
    int init_process();
    int check_OCR();
    int obtain_card_info();
    int set_block_length();
    int populate_block(size_t new_block);
    int flush_block();
    int traverse_data(size_t count, const std::function< void(size_t data_offt,
                                                              size_t blk_offt,
                                                              size_t amount) >& fn);

    // Useful abstractions
    int receive_response(R1 &r);
    int receive_response(R3 &r);
    //int receive_response(R1_read &r);

    int receive_data(uint8_t *buf, size_t size);
    int send_data(const uint8_t *buf, size_t size);

    // Transport layer TODO: merge these three
    ssize_t spi_send(const uint8_t *buf, size_t size);
    ssize_t spi_receive(uint8_t *buf, size_t size);
    ssize_t spi_send_dummy(size_t size);

    bool          m_inited;     // Inited flags
    bool          m_HC;         // High Capacity flag
    int           m_opened;     // Opened times counter
    off_t         m_offt;       // Current offset in units of bytes
    block_buffer  m_block;      // Buffer containing last read\write block
};

template< class spi_dev, class GPIO_CS >
sd_spi< spi_dev, GPIO_CS >::sd_spi()
    :m_inited{false}
    ,m_HC{false}
    ,m_opened{0}
    ,m_offt{0}
    ,m_block{}
{
}

template< class spi_dev, class GPIO_CS >
sd_spi< spi_dev, GPIO_CS >::~sd_spi()
{
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::init()
{
    if (!m_inited) {
        spi_dev::init();

        spi_dev::lock();
        GPIO_CS::set();

        spi_dev::unlock();

        m_inited = true;
    }


    return 0;
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::open()
{
    if (!m_inited) {
        return -1;
    }

    int ret = 0;

    if (!m_opened) {
        spi_dev::lock();


        if (send_init() < 0) {
            ecl::cout << "Send init clocks failed" << ecl::endl;
            ret = -1;
        } else {
            GPIO_CS::reset();
            if (open_card() < 0) {
                ret = -2;
            }
            GPIO_CS::set();
        }

        spi_dev::unlock();
    }

    m_opened++;
    return ret;
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::close()
{
    if (!--m_opened) {
        // TODO
    }

    return 0;
}

template< class spi_dev, class GPIO_CS >
ssize_t sd_spi< spi_dev, GPIO_CS >::write(const uint8_t *data, size_t count)
{
    if (!m_opened) {
        return -1;
    }

    int SD_ret;
    auto fn = [data, this](size_t data_offt, size_t blk_offt, size_t to_copy) {
        memcpy(this->m_block.block + blk_offt, data + data_offt, to_copy);
        this->m_block.mint = false;
    };

    SD_ret = traverse_data(count, fn);
    if (SD_ret < 0)
        return SD_ret;

    return count;
}

template< class spi_dev, class GPIO_CS >
ssize_t sd_spi< spi_dev, GPIO_CS >::read(uint8_t *data, size_t count)
{
    if (!m_opened) {
        return -1;
    }

    int SD_ret;
    auto fn = [data, this](size_t data_offt, size_t blk_offt, size_t to_copy) {
        memcpy(data + data_offt, this->m_block.block + blk_offt , to_copy);
    };

    SD_ret = traverse_data(count, fn);
    if (SD_ret < 0)
        return SD_ret;

    return count;
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::flush()
{
    int ret = 0;

    spi_dev::lock();
    GPIO_CS::reset();

    if (flush_block() < 0)
        ret = -1;

    spi_dev::unlock();
    GPIO_CS::set();

    return ret;
}

// TODO: change off_t to int
template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::seek(off_t offset)
{
    if (!m_opened) {
        return -1;
    }

    m_offt = offset;
    return 0;
}


template< class spi_dev, class GPIO_CS >
off_t sd_spi< spi_dev, GPIO_CS >::tell() const
{
    if (!m_opened) {
        return -1;
    }

    return m_offt;
}

template< class spi_dev, class GPIO_CS >
constexpr size_t sd_spi< spi_dev, GPIO_CS >::get_block_length()
{
    return block_buffer::block_len;
}

// Private methods -------------------------------------------------------------

template< class spi_dev, class GPIO_CS >
ssize_t sd_spi< spi_dev, GPIO_CS >::spi_send(const uint8_t *buf, size_t size)
{
    spi_dev::set_buffers(buf, nullptr, size);
    // TODO: check error code
    spi_dev::xfer(&size, nullptr);

    // TODO: verify that all data was transfered
    return size;
}

template< class spi_dev, class GPIO_CS >
ssize_t sd_spi< spi_dev, GPIO_CS >::spi_receive(uint8_t *buf, size_t size)
{
    spi_dev::set_buffers(nullptr, buf, size);
    // TODO: check error code
    spi_dev::xfer(nullptr, &size);

    // TODO: verify that all data was transfered
    return size;
}

template< class spi_dev, class GPIO_CS >
ssize_t sd_spi< spi_dev, GPIO_CS >::spi_send_dummy(size_t size)
{
    spi_dev::set_buffers(size);
    // TODO: check error code
    spi_dev::xfer(&size, nullptr);

    // TODO: verify that all data was transfered
    return size;
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::send_init()
{
    /* Initialise card with >= 74 clocks on start */
    ssize_t ret = spi_send_dummy(80);
    if (ret < 0)
        return ret;

    return sd_ok;
}

template< class spi_dev, class GPIO_CS >
template< typename R >
int sd_spi< spi_dev, GPIO_CS >::send_CMD(R &resp, uint8_t CMD_idx, const argument &arg, uint8_t crc)
{
    ssize_t sd_ret;

    CMD_idx &= 0x3f; // First two bits are reserved TODO: comment
    CMD_idx |= 0x40;

    crc |= 0x1; // EOT flag

    // Init a transaction
    spi_send_dummy(1);

    // Command body
    const uint8_t to_send[] =
    { CMD_idx, arg[0], arg[1], arg[2], arg[3], crc };

    // Send HCS
    sd_ret = spi_send(to_send, sizeof(to_send));
    if (sd_ret < 0) {
        return sd_ret;
    }

    // Retrieve a result
    sd_ret = receive_response(resp);
    if (sd_ret < 0) {
        return sd_ret;
    }

    return (sd_ret < 0) ? sd_ret : sd_ok;
}

//------------------------------------------------------------------------------

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::receive_response(R1 &r)
{
    uint8_t tries = 8;
    int ret;

    do {
        ret = spi_receive(&r.response, 1);
        if (ret < 0)
            return ret;

    } while (!r.received() && --tries);

    if (!r.received()) {
        ecl::cout << "R1 response wait expired." << ecl::endl;
    } else if (!r.ok()) {
        ecl::cout << "R1 error(s) received:" << ecl::endl;

        if (r.response & R1::erase_reset)
            ecl::cout << "Erase reset error" << ecl::endl;
        if (r.response & R1::illegal_cmd)
            ecl::cout << "Illegal command error" << ecl::endl;
        if (r.response & R1::com_crc_err)
            ecl::cout << "CRC error" << ecl::endl;
        if (r.response & R1::erase_seq_err)
            ecl::cout << "Erase sequnce error" << ecl::endl;
        if (r.response & R1::addr_err)
            ecl::cout << "Address error" << ecl::endl;
        if (r.response & R1::param_error)
            ecl::cout << "Parameter error" << ecl::endl;

    } else {
        return sd_ok;
    }

    return sd_err;
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::receive_response(R3 &r)
{
    int SD_ret = receive_response(r.r1);
    if (SD_ret < 0)
        return SD_ret;

    return spi_receive((uint8_t *)&r.OCR, sizeof(r.OCR));
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::receive_data(uint8_t *buf, size_t size)
{
    // Data token that returned in case of success
    static constexpr uint8_t data_token    = 0xfe;

    // Flags that can be found in error token
    static constexpr uint8_t err           = 0x01;
    static constexpr uint8_t CC_err        = 0x02;
    static constexpr uint8_t ECC_fail      = 0x04;
    static constexpr uint8_t out_of_range  = 0x08;
    static constexpr uint8_t card_locked   = 0x10;

    uint8_t  token = 0;
    uint16_t crc;
    uint8_t  tries = 64;
    int      SD_ret;

    do {
        SD_ret = spi_receive(&token, sizeof(token));
        if (SD_ret < 0)
            return SD_ret;
    } while (token == 0xff && --tries);

    if (!tries) {
        ecl::cout << "Timeout waiting for data token" << ecl::endl;
        return sd_err;
    }

    // Data token received,
    if (token == data_token) {
        SD_ret = spi_receive(buf, size);
        if (SD_ret < 0) {
            return SD_ret;
        }
    } else {
        ecl::cout << "Error token " << token
                  << " received when reading data:" << ecl::endl;
        if (token & err) {
            ecl::cout << "General error occurs" << ecl::endl;
        }
        if (token & CC_err) {
            ecl::cout << "CC error" << ecl::endl;
        }
        if (token & ECC_fail) {
            ecl::cout << "ECC failed" << ecl::endl;
        }
        if (token & out_of_range) {
            ecl::cout << "Out of range error" << ecl::endl;
        }
        if (token & card_locked) {
            ecl::cout << "Card locked" << ecl::endl;
        }

        return sd_err;
    }

    return spi_receive((uint8_t *)&crc, sizeof(crc));
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::send_data(const uint8_t *buf, size_t size)
{
    // Data token that must be sent before data chunk
    static constexpr uint8_t data_token    = 0xfe;

    // Two bits indicating data response
    static constexpr uint8_t mask          = 0x11;
    // Flags that can be found in data response
    static constexpr uint8_t accepted      = 0x05;
    static constexpr uint8_t crc_err       = 0x0b;
    static constexpr uint8_t write_err     = 0x0d;

    static constexpr uint8_t crc           = 0x0;

    uint8_t  data_response = 0;
    uint8_t  tries = 32;
    int      sd_ret;

    // Token
    if ((sd_ret = spi_send(&data_token, sizeof(data_token))) < 0) {
        return sd_ret;
    }

    // Data itself
    if ((sd_ret = spi_send(buf, size)) < 0) {
        return sd_ret;
    }

    // Dummy CRC
    if ((sd_ret = spi_send(&crc, sizeof(crc))) < 0) {
        return sd_ret;
    }

    // Wait for data response
    do {
        sd_ret = spi_receive(&data_response, sizeof(data_response));
        if (sd_ret < 0)
            return sd_ret;
    } while (((data_response & mask) != 0x1) && --tries);

    // No error occur, only 4 lower bits matters
    if ((data_response & 0x0f) == accepted) {
        // Wait till card is busy
        do {
            sd_ret = spi_receive(&data_response, sizeof(data_response));
            if (sd_ret < 0)
                return sd_ret;
        } while (data_response == 0x0);

        return sd_ok;
    }

    ecl::cout << "Error in data response " << data_response << ecl::endl;
    if (data_response & crc_err) {
        ecl::cout << "CRC error occurs" << ecl::endl;
    }
    if (data_response & write_err) {
        ecl::cout << "Write error" << ecl::endl;
    }

    return sd_err;
}

//------------------------------------------------------------------------------

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::CMD0(R1 &r)
{
    // TODO: comments
    constexpr uint8_t  CMD0_idx = 0;
    constexpr uint8_t  CMD0_crc = 0x95;
    constexpr argument arg      = {0};
    return send_CMD(r, CMD0_idx, arg, CMD0_crc);
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::CMD8(R7 &r)
{
    // TODO: comments
    constexpr uint8_t   CMD8_idx = 8;
    constexpr uint8_t   CMD8_crc = 0x87;
    constexpr argument  arg      = { 0, 0, 0x01, 0xaa };
    return send_CMD(r, CMD8_idx, arg, CMD8_crc);
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::CMD10(R1 &r)
{
    // TODO: comments
    constexpr uint8_t   CMD10_idx  = 10;
    constexpr argument  arg        = { 0 };
    return send_CMD(r, CMD10_idx, arg);

}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::CMD16(R1 &r)
{
    // TODO: comments
    constexpr uint8_t  CMD16_idx = 16;
    constexpr uint32_t block_len = block_buffer::block_len;
    constexpr argument arg = {
        (uint8_t) (block_len >> 24),
        (uint8_t) (block_len >> 16),
        (uint8_t) (block_len >> 8),
        (uint8_t) (block_len),
    };
    return send_CMD(r, CMD16_idx, arg);
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::CMD17(R1 &r, uint32_t address)
{
    // TODO: comments
    constexpr uint8_t CMD17_idx = 17;
    const argument arg = {
        (uint8_t) (address >> 24),
        (uint8_t) (address >> 16),
        (uint8_t) (address >> 8),
        (uint8_t) (address),
    };
    return send_CMD(r, CMD17_idx, arg);
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::CMD24(R1 &r, uint32_t address)
{
    constexpr uint8_t CMD24_idx = 24;
    const argument arg = {
        (uint8_t) (address >> 24),
        (uint8_t) (address >> 16),
        (uint8_t) (address >> 8),
        (uint8_t) (address),
    };
    return send_CMD(r, CMD24_idx, arg);
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::CMD55(R1 &r)
{
    // TODO: comments
    constexpr uint8_t CMD55_idx = 55;
    constexpr argument arg      = { 0, 0, 0, 0 };
    return send_CMD(r, CMD55_idx, arg);
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::CMD58(R3 &r)
{
    // TODO: comments
    constexpr uint8_t CMD58_idx = 58;
    constexpr argument arg      = { 0, 0, 0, 0 };
    return send_CMD(r, CMD58_idx, arg);
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::ACMD41(R1 &r, bool HCS)
{
    const uint8_t HCS_byte = HCS ? (1 << 6) : 0;

    int SD_ret = CMD55(r);
    if (SD_ret < 0)
        return SD_ret;

    // TODO: comments
    constexpr uint8_t  CMD41_idx  = 41;
    const argument     arg        = { HCS_byte, 0, 0, 0 };
    return send_CMD(r, CMD41_idx, arg);
}

//------------------------------------------------------------------------------

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::open_card()
{
    int SD_ret;
    if ((SD_ret = software_reset()) < 0) {
        ecl::cout << "Failed to reset a card" << ecl::endl;
        return SD_ret;
    }

    if ((SD_ret = check_conditions()) < 0) {
        ecl::cout << "Failed to check conditions" << ecl::endl;
        return SD_ret;
    }

    if ((SD_ret = init_process()) < 0) {
        ecl::cout << "Failed to init a card" << ecl::endl;
        return SD_ret;
    }

    SD_ret = check_OCR();
    if (SD_ret < 0) {
        ecl::cout << "Failed to check OCR" << ecl::endl;
        return SD_ret;
    } else if (SD_ret == sd_type_hc) {
        m_HC = true;
    } else if (SD_ret == sd_type_sc) {
        if (set_block_length() < 0) {
            ecl::cout << "Can't set block length" << ecl::endl;
            return SD_ret;
        }
    } else {
        ecl::cout << "Unsupported type of card" << ecl::endl;
        return sd_err;
    }

    ecl::cout << "Card initialized successfully" << ecl::endl;

    if ((SD_ret = obtain_card_info()) < 0) {
        ecl::cout << "Failed to obtain card info" << ecl::endl;
        return SD_ret;
    }

    if ((SD_ret = populate_block(m_offt)) < 0) {
        ecl::cout << "Cannot populate initial block" << ecl::endl;
    }

    return SD_ret;
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::software_reset()
{
    R1 r1;

    int SD_ret = CMD0(r1);
    if (SD_ret < 0)
        return SD_ret;

    if (r1.response & R1::idle_state) {
        ecl::cout << "Card is in idle state" << ecl::endl;
        return sd_ok;
    }

    return sd_err;
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::check_conditions()
{
    R7 r7;

    int SD_ret = CMD8(r7);
    if (SD_ret < 0)
        return SD_ret;

    uint32_t OCR = ecl::BE(r7.OCR);

    if (r7.r1.response & R1::idle_state) {
        if ((OCR & 0xff) != 0xaa) {
            ecl::cout << "Check pattern mismatch" << ecl::endl;
            return sd_err;
        }

        if (!(OCR & 0x100)) {
            ecl::cout << "Voltage range not accepted" << ecl::endl;
            return sd_err;
        }

        return sd_ok;
    }

    return sd_err;
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::init_process()
{
    //TODO: comments
    R1 r1;
    int SD_ret;

    r1.response = 0x1;
    uint8_t tries = 255;

    while ((r1.response & R1::idle_state) && --tries) {
        SD_ret = ACMD41(r1, true);

        if (SD_ret < 0) {
            return SD_ret;
        }
    }

    if (!tries) {
        ecl::cout << "Card wasn't initialized within given period" << ecl::endl;
        return sd_err;
    }

    return sd_ok;
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::check_OCR()
{
    R3 r3;
    CMD58(r3);
    int SD_ret = 0;

    uint32_t OCR = ecl::BE(r3.OCR);

    // TODO: compare current profile and supported one!
    // Otherwise we can damage the card
    if (!r3.r1.response) {
        // Card sends data (as well as OCR) in BE format
        if (OCR & (1 << 31)) {
            ecl::cout << "Card is powered-up" << ecl::endl;
            if (OCR & (1 << 30)) {
                ecl::cout << "Card is High capacity" << ecl::endl;
                SD_ret = sd_type_hc;
            } else {
                ecl::cout << "Card is Standard capacity" << ecl::endl;
                SD_ret = sd_type_sc;
            }
        }

        // Whole byte 2 of OCR contains voltage profiles
        for (uint8_t bit = 15; bit < 24; ++bit) {
            if (OCR & (1 << bit)) {
                uint8_t V = 27 + bit - 15;
                uint intgr = V / 10;
                uint fract = V - intgr * 10;
                ecl::cout << "VOLTAGE: ["
                          << intgr << '.' << fract
                          << "; +0.1]" << ecl::endl;
            }
        }

        return SD_ret;
    }

    return sd_err;
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::obtain_card_info()
{
    R1  r1;
    int SD_ret;
    uint8_t cid[16];

    if ((SD_ret = CMD10(r1)) < 0) {
        return SD_ret;
    }

    if ((SD_ret = receive_data(cid, sizeof(cid))) < 0) {
        return SD_ret;
    }

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
    return sd_ok;
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::set_block_length()
{
    R1 r1;

    int SD_ret = CMD16(r1);
    if (SD_ret < 0)
        return SD_ret;

    if (r1.response & R1::idle_state) {
        ecl::cout << "Card is in idle state during CMD16" << ecl::endl;
        SD_ret = sd_err;
    }

    return SD_ret;
}


template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::populate_block(size_t new_block)
{
    R1 r1;
    off_t address = m_HC ? new_block : new_block * block_buffer::block_len;
    int SD_ret = flush_block();

    if ((SD_ret = CMD17(r1, address)) < 0) {
        return SD_ret;
    }

    if ((SD_ret = receive_data(m_block.block, block_buffer::block_len)) < 0) {
        return SD_ret;
    }

    m_block.origin = new_block;
    return SD_ret;
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::flush_block()
{
    int SD_ret;
    R1 r1;

    if (m_block.mint) {
        return sd_ok;
    }

    off_t address = m_HC ? m_block.origin : m_block.origin * block_buffer::block_len;

    if ((SD_ret = CMD24(r1, address)) < 0) {
        return SD_ret;
    }

    if ((SD_ret = send_data(m_block.block, block_buffer::block_len)) < 0) {
        return SD_ret;
    }

    m_block.mint = true;
    return SD_ret;
}

template< class spi_dev, class GPIO_CS >
int sd_spi< spi_dev, GPIO_CS >::traverse_data(
        size_t count,
        const std::function< void (size_t, size_t, size_t) > &fn
        )
{
    size_t left = count;
    int SD_ret = sd_ok;

    // Intended to be optimized in right shift, sinse
    // block length is constant and a power of two
    size_t blk_num = m_offt / block_buffer::block_len;
    size_t blk_offt = m_offt - blk_num  * block_buffer::block_len;
    size_t data_offt = 0;

    while (left) {
        if (blk_num  != m_block.origin) {
            spi_dev::lock();
            GPIO_CS::reset();
            SD_ret = populate_block(blk_num);
            GPIO_CS::set();
            spi_dev::unlock();

            if (SD_ret < 0)
                return SD_ret;
        }

        size_t to_copy = std::min(block_buffer::block_len - blk_offt , left);
        // Copy data
        fn(data_offt, blk_offt, to_copy);

        // Advance to next free chunk
        data_offt += to_copy;
        left -= to_copy;

        // If needed, next iteration will populate buffer again
        blk_num ++;
        // Next copy will occur within a new block
        blk_offt  = 0;
    }

    m_offt += count;

    return SD_ret;
}

} // namespace ecl

#endif // DEV_SDSPI_H
