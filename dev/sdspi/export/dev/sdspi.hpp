/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief SDSPI driver module
//! \details Obtain SDSPI reference here: https://www.sdcard.org/downloads/pls/

#ifndef DEV_SDSPI_HPP
#define DEV_SDSPI_HPP

#include <algorithm>
#include <array>
#include <type_traits>
#include <cstring>

#include <ecl/iostream.hpp>
#include <ecl/endian.hpp>
#include <ecl/types.h>

namespace ecl
{

//! \addtogroup dev External device drivers
//! @{

//! \defgroup sdspi SDSPI driver module
//! @{

//! Struct to hold SDSPI driver state, resulting from SDSPI API calls.
//! \details Used primarily for debugging.
struct sdspi_state
{
    inline bool is_ok() const                       { return !err_state
                                                                && !recv_tok
                                                                && !send_resp
                                                                && !(r1_resp & 0xfe); }
    inline void clear()                             { r1_resp       = 0;
                                                      recv_tok      = 0;
                                                      err_state     = 0;
                                                      send_resp     = 0; }

    // Generic state flags

    inline bool err_send_resp_expired() const       { return err_state & 0x1; }
    inline bool err_recv_tok_expired() const        { return err_state & 0x2; }
    inline bool err_invalid_state() const           { return err_state & 0x4; }
    inline bool err_pattern_mismatch() const        { return err_state & 0x8; }
    inline bool err_voltage_range() const           { return err_state & 0x10; }
    inline bool err_init_expired() const            { return err_state & 0x20; }
    inline bool err_r1_expired() const              { return err_state & 0x40; }
    inline bool err_unsup_card() const              { return err_state & 0x80; }

    inline void set_err_send_resp_expired()         { err_state |= 0x1;    }
    inline void set_err_recv_tok_expired()          { err_state |= 0x2;    }
    inline void set_err_invalid_state()             { err_state |= 0x4;    }
    inline void set_err_pattern_mismatch()          { err_state |= 0x8;    }
    inline void set_err_voltage_range()             { err_state |= 0x10;   }
    inline void set_err_init_expired()              { err_state |= 0x20;   }
    inline void set_err_r1_expired()                { err_state |= 0x40;   }
    inline void set_err_unsup_card()                { err_state |= 0x80;   }

    // R1 flags

    inline bool idle_state() const                  { return r1_resp & 0x1;  }
    inline bool err_erase_reset() const             { return r1_resp & 0x2;  }
    inline bool err_illegal_cmd() const             { return r1_resp & 0x4;  }
    inline bool err_com_crc() const                 { return r1_resp & 0x8;  }
    inline bool err_erase_seq() const               { return r1_resp & 0x10; }
    inline bool err_addr_err() const                { return r1_resp & 0x20; }
    inline bool err_param() const                   { return r1_resp & 0x40; }

    inline void set_r1_flags(uint8_t r1)            { r1_resp = r1; }

    inline void set_idle_state()                    { r1_resp |= 0x1;   }
    inline void set_err_erase_reset()               { r1_resp |= 0x2;   }
    inline void set_err_illegal_cmd()               { r1_resp |= 0x4;   }
    inline void set_err_com_crc()                   { r1_resp |= 0x8;   }
    inline void set_err_erase_seq()                 { r1_resp |= 0x10;  }
    inline void set_err_addr_err()                  { r1_resp |= 0x20;  }
    inline void set_err_param()                     { r1_resp |= 0x40;  }

    // Receive data token flags

    inline bool err_generic() const                 { return recv_tok & 0x1;  }
    inline bool err_cc() const                      { return recv_tok & 0x2;  }
    inline bool err_ecc() const                     { return recv_tok & 0x4;  }
    inline bool err_out_of_rage() const             { return recv_tok & 0x8;  }
    inline bool err_lock() const                    { return recv_tok & 0x10; }

    inline void set_recv_tok_flags(uint8_t tok)     { recv_tok = tok;    }

    inline void set_err_generic()                   { recv_tok |= 0x1;  }
    inline void set_err_cc()                        { recv_tok |= 0x2;  }
    inline void set_err_ecc()                       { recv_tok |= 0x4;  }
    inline void set_err_out_of_rage()               { recv_tok |= 0x8;  }
    inline void set_err_lock()                      { recv_tok |= 0x10; }

    // Receive data response flags

    inline bool err_crc() const                     { return send_resp & 0x0b; }
    inline bool err_write() const                   { return send_resp & 0x0d; }

    inline void set_send_resp_flags(uint8_t f)      { send_resp = f;    }

    inline void set_err_crc()                       { send_resp |= 0x0b;  }
    inline void set_err_write()                     { send_resp |= 0x0d;  }

    //! Prints contents of the struct to the specified stream.
    template<class dev>
    friend ostream<dev> &operator<< (ostream<dev> &left, const sdspi_state &s)
    {
        if (s.idle_state()) {
            left << "Card is in idle state" << ecl::endl;
        }

        if (s.is_ok()) {
            return left;
        }

        if (s.err_r1_expired()) {
            left << "R1 response wait expired." << ecl::endl;
        }

        // R1 flags

        if (s.r1_resp & 0xfe) {
            left << "R1 response error:" << ecl::endl;
        }

        if (s.err_erase_reset()) {
            left << "erase reset error" << ecl::endl;
        }
        if (s.err_illegal_cmd()) {
            left << "illegal command" << ecl::endl;
        }
        if (s.err_com_crc()) {
            left << "CRC error" << ecl::endl;
        }
        if (s.err_erase_seq()) {
            left << "erase sequence error" << ecl::endl;
        }
        if (s.err_addr_err()) {
            left << "address error" << ecl::endl;
        }
        if (s.err_param()) {
            left << "parameter error" << ecl::endl;
        }

        // Unexpected states and logical protocol errors

        if (s.err_state) {
            left << "Logic error:" << ecl::endl;
        }

        if (s.err_send_resp_expired()) {
            left << "timeout waiting for data send response" << ecl::endl;
        }

        if (s.err_send_resp_expired()) {
            left << "timeout when initializing a card" << ecl::endl;
        }

        if (s.err_invalid_state()) {
            left << "invalid card state" << ecl::endl;
        }

        // TODO: not really logical error, but rather OCR
        if (s.err_pattern_mismatch()) {
            left << "check pattern mismatch" << ecl::endl;
        }

        // TODO: not really logical error, but rather OCR
        if (s.err_voltage_range()) {
            left << "voltage range is not supported" << ecl::endl;
        }

        if (s.err_unsup_card()) {
            left << "unsupported type of card" << ecl::endl;
        }

        // Reading data error

        if (s.recv_tok) {
            left << "Error while reading data:" << ecl::endl;
        }

        if (s.err_generic()) {
            ecl::cout << "generic read error" << ecl::endl;
        }
        if (s.err_cc()) {
            ecl::cout << "CC error" << ecl::endl;
        }
        if (s.err_ecc()) {
            ecl::cout << "ECC failed" << ecl::endl;
        }
        if (s.err_out_of_rage()) {
            ecl::cout << "out of range" << ecl::endl;
        }
        if (s.err_lock()) {
            ecl::cout << "card locked" << ecl::endl;
        }

        // Writing data error

        if (s.send_resp) {
            left << "Error while writing data:" << ecl::endl;
        }

        if (s.err_crc()) {
            left << "CRC error" << ecl::endl;
        }

        if (s.err_write()) {
            left << "write error" << ecl::endl;
        }

        return left;
    }

    uint8_t err_state;
    uint8_t r1_resp;
    uint8_t recv_tok;
    uint8_t send_resp;
};

//! SDSPI card information struct.
//! \details Used primarily for debugging.
struct sdspi_card_info
{
    uint8_t   mid;        //!< Card manufacturer ID.
    char      oid[3];     //!< OEM/APP ID. 2 chars string, last one is for NUL
    char      pnm[6];     //!< Product name. 5 chars string, last one is for NUL.
    uint8_t   prv_high;   //!< Extracted product revision number, high.
    uint8_t   prv_low;    //!< Extracted product revision number, low.
    uint32_t  psn;        //!< 32 bits wide serial number.
    uint16_t  mdt_year;   //!< The manufacturing date, year.
    uint8_t   mdt_month;  //!< The manufacturing date, month.
    bool      type_hc;    //!< True if card is high capacity.

    template<class dev>
    friend ostream<dev> &operator<< (ostream<dev> &left, const sdspi_card_info &s)
    {
        left << "------------------------------------" << ecl::endl;
        left << "Manufacturer ID:       " << s.mid << ecl::endl;
        left << "OEM/Application ID:    " << s.oid << ecl::endl;
        left << "Product name:          " << s.pnm << ecl::endl;
        left << "Product revision:      " << s.prv_high << '.' << s.prv_low << ecl::endl;
        left << "S/N:                   " << (unsigned)s.psn << ecl::endl;
        left << "Manufacturing date:    " << s.mdt_month << '.' << s.mdt_year << ecl::endl;
        left << "High capacity:         " << (int)s.type_hc << ecl::endl;
        left << "------------------------------------" << ecl::endl;

        return left;
    }
};

//! SDSPI driver class
//! \tparam spi_dev  SPI bus driver
//! \tparam gpio_cs  Chip-select GPIO
//! \todo mention about 8 additional clocks before each command!!!
//! \details Driver follows SDSPI specification that can be obtained here:
//! https://www.sdcard.org/downloads/pls/
template<class spi_dev, class gpio_cs>
class sdspi
{
public:
    //! Initializes SDSPI driver and SD card.
    //! \pre Un-initialized SDSPI driver.
    //! \post Initialized SDSPI driver. Offset is set to 0.
    //!       State obtainable via get_state() is populated
    //!       with relevant data.
    //! \retval err::io         I/O error on SPI.
    //! \retval err::generic    Operation failed. Use get_state() to
    //!                         get more information.
    //! \retval err::ok         Operation succeed.
    static err init();

    //! De-initializes SDSPI driver and SD card.
    //! \pre Initialized SDSPI driver.
    //! \post Uninitialized SDSPI driver.
    //! \retval err::io         I/O error on SPI.
    //! \retval err::generic    Operation failed. Use get_state() to
    //!                         get more information.
    //! \retval err::ok         Operation succeed.
    static err deinit();

    //! Writes given data to SD card from current offset.
    //! \pre  Initialized SDSPI driver.
    //! \post Current offset is increased by the amount of data written.
    //!       State obtainable via get_state() is populated
    //!       with relevant data.
    //! \todo how to catch end-of-space on SD card?
    //! \details Data written may be a subject to caching. Use flush()
    //! call to make sure that all data is written to SD card.
    //! \param[in]      data    Data to write.
    //! \param[in,out]  count   Data size in bytes to write on entry.
    //!                         Data bytes actually written on exit.
    //! \retval err::io         I/O error on SPI.
    //! \retval err::generic    Operation failed. Use get_state() to
    //!                         get more information.
    //! \retval err::ok         Operation succeed.
    static err write(const uint8_t *data, size_t &count);

    //! Reads data from SD card from current offset to given buffer.
    //! \pre  Initialized SDSPI driver.
    //! \post Current offset is increased by the amount of data read.
    //!       State obtainable via get_state() is populated
    //!       with relevant data.
    //! \todo how to catch end-of-space on SD card?
    //! \param[out]     data    Buffer to store data into.
    //! \param[in,out]  count   Data size in bytes to read on entry.
    //!                         Data bytes actually read on exit.
    //! \retval err::io         I/O error on SPI.
    //! \retval err::generic    Operation failed. Use get_state() to
    //!                         get more information.
    //! \retval err::ok         Operation succeed.
    static err read(uint8_t *data, size_t &count);

    //! Flushes cached data.
    //! \pre  Initialized SDSPI driver.
    //! \post Data from cache is flushed onto SD card.
    //!       Current offset is unchanged.
    //!       State obtainable via get_state() is populated
    //!       with relevant data.
    //! \todo how to catch end-of-space on SD card?
    //! \retval err::io         I/O error on SPI.
    //! \retval err::generic    Operation failed. Use get_state() to
    //!                         get more information.
    //! \retval err::ok         Operation succeed.
    static err flush();

    //! Seeks to the given position, in bytes.
    //! \pre  Initialized SDSPI driver.
    //! \post Offset is changed to given value.
    //!       State obtainable via get_state() is populated
    //!       with relevant data.
    //! \todo extend it with seekdir::cur and seekdir::end
    //! \todo how to catch end-of-space on SD card?
    //! \param[in] offt         New offset in bytes.
    //! \retval err::io         I/O error on SPI.
    //! \retval err::generic    Operation failed. Use get_state() to
    //!                         get more information.
    //! \retval err::ok         Operation succeed.
    static err seek(off_t offt /*, seekdir::set */);

    //! Tells current position
    //! \pre  Initialized SDSPI driver.
    //! \post State obtainable via get_state() is populated
    //!       with relevant data.
    //! \param[out] offt        Current offset.
    //! \retval err::io         I/O error on SPI.
    //! \retval err::generic    Operation failed. Use get_state() to
    //!                         get more information.
    //! \retval err::ok         Operation succeed.
    static err tell(off_t &offt);

    //! Gets card info.
    //! \pre  Initialized SDSPI driver.
    //! \post SD card queried for card information.
    //!       State obtainable via get_state() is populated
    //!       with relevant data.
    //! \param[out] info        Card info.
    //! \retval err::io         I/O error on SPI.
    //! \retval err::generic    Operation failed. Use get_state() to
    //!                         get more information.
    //! \retval err::ok         Operation succeed.
    static err get_info(sdspi_card_info &info);

    //! Gets state of the driver, resulted from last API call.
    //! \pre  Driver in any state (either successfully or unsuccessfully
    //!       initialized).
    //! \post State obtainable via get_state() is populated
    //!       with relevant data.
    //! \return Currect driver state.
    static const sdspi_state &get_state();

    //! Returns a length of a block
    static constexpr size_t get_block_length();

    //! Seek flags
    enum class seekdir
    {
        beg = 0, //! Seek relative to the beginning.
        cur = 1, //! Seek relative to the current position.
        end = 2  //! Seek relative to the end.
    };

private:
    //! Card types
    enum class sd_type
    {
        hc,   //! The card is high capacity
        sc,   //! The card is standart capacity
        unkn, //! Unknown card type
    };

    //! R1 response
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

    //! R3 response
    struct R3
    {
        R3()      :r1{}, OCR{0} {}

        R1         r1;
        uint32_t   OCR;
    };

    //! R7 and R3 layouts are the same
    using R7 = R3;
    using argument = std::array<uint8_t, 4>;

    // http://elm-chan.org/docs/mmc/mmc_e.html
    // SD SPI command set ------------------------------------------------------

    //! GO_IDLE_STATE            - Software reset.
    static err CMD0(R1 &r);

    //! SEND_OP_COND             - Initiate initialization process.
    static err CMD1(R1 &r);

    //! APP_SEND_OP_COND         - For only SDC. Initiate initialization process.
    static err ACMD41(R1 &r, bool HCS);

    //! SEND_IF_COND             - For only SDC V2. Check voltage range.
    static err CMD8(R7 &r);

    //! SEND_CSD                 - Read CSD register.
    static err CMD9(R1 &r, uint8_t *buf, size_t size);

    //! SEND_CID                 - Read CID register.
    static err CMD10(R1 &r);

    // STOP_TRANSMISSION        - Stop to read data.
    // int CMD12(R1 &r); R1b is required here

    //! SET_BLOCKLEN             - Change R/W block size.
    static err CMD16(R1 &r);

    //! READ_SINGLE_BLOCK        - Read a block.
    static err CMD17(R1 &r, uint32_t address);

    //! READ_MULTIPLE_BLOCK      - Read multiple blocks.
    static err CMD18(R1 &r, uint32_t address, uint8_t *buf, size_t size);

    //! SET_BLOCK_COUNT          - For only MMC. Define number of blocks to transfer
    //!                            with next multi-block read/write command.
    static err CMD23(R1 &r, uint16_t block_count);

    //! SET_WR_BLOCK_ERASE_COUNT - For only SDC. Define number of blocks to pre-erase
    //!                              with next multi-block write command.
    static err ACMD23(R1 &r, uint16_t block_count);

    //! WRITE_BLOCK              - Write a block.
    static err CMD24(R1 &r, uint32_t address);

    //! WRITE_MULTIPLE_BLOCK     - Write multiple blocks.
    static err CMD25(R1 &r, uint32_t address, uint8_t *buf, size_t size);

    //! APP_CMD                  - Leading command of ACMD<n> command.
    static err CMD55(R1 &r);

    //! READ_OCR                 - Read OCR.
    static err CMD58(R3 &r);

    //! Sends CMD
    template< typename R >
    static err send_CMD(R &resp, uint8_t CMD_idx, const argument &arg, uint8_t crc = 0);

    //! Sends >= 47 empty clocks to initialize the device
    static err send_init();
    static err open_card();
    static err software_reset();
    static err check_conditions();
    static err init_process();
    static err check_OCR(sd_type &type);
    static err obtain_card_info();
    static err set_block_length();
    static err populate_block(size_t new_block);
    static err flush_block();
    static err traverse_data(size_t count,
        const std::function<void(size_t data_offt, size_t blk_offt, size_t amount)>& fn);

    // Useful abstractions
    static err receive_response(R1 &r);
    static err receive_response(R3 &r);
    //err receive_response(R1_read &r);

    static err receive_data(uint8_t *buf, size_t size);
    static err send_data(const uint8_t *buf, size_t size);

    // Transport layer TODO: merge these three
    static err spi_send(const uint8_t *buf, size_t size);
    static err spi_receive(uint8_t *buf, size_t size);
    static err spi_send_dummy(size_t size);


    //! Context POD-type, allocated as a static object.
    //! \details All fields initialized to 0 during static object initialization
    static struct ctx_type
    {
        bool          inited;     // Init flag
        bool          hc;         // High Capacity flag
        off_t         offt;       // Current offset in units of bytes
        sdspi_state   state;      // Last state of operation

        //! Block POD-type, allocated as an subobject of a static object.
        //! \details  All fields initialized to 0 during static object initialization
        struct block_type
        {
            // Block length is given without respect to the card type.
            // If card is Standart Capacity then block length will be set to 512,
            // ( see set_block_length() ).
            // If card is High Capacity then block length is fixed to 512 by design.
            static constexpr size_t block_len = 512;

            uint8_t buf[block_len];       //!< The block itself
            size_t  origin;               //!< The offset from which block was obtained
            bool    mint;                 //!< True if there were no writes in buffer
        } block;    // Cache containing last read\write block
    } m_ctx;

    // Protect from future bugs
    static_assert(std::is_trivial<ctx_type>::value, "Context type must be trival");
};

template<class spi_dev, class gpio_cs>
typename sdspi<spi_dev, gpio_cs>::ctx_type sdspi<spi_dev, gpio_cs>::m_ctx = {};

//------------------------------------------------------------------------------

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::init()
{
    err rc;
    ecl_assert(!m_ctx.inited);
    m_ctx.block.mint = true;
    m_ctx.state.clear();

    rc = spi_dev::init();
    if (is_error(rc)) {
        return rc;
    }

    spi_dev::lock();
    gpio_cs::set();
    spi_dev::unlock();

    spi_dev::lock();
    rc = send_init();

    if (is_ok(rc)) {
        gpio_cs::reset();
        rc = open_card();
        gpio_cs::set();
    }

    spi_dev::unlock();

    if (is_ok(rc)) {
        m_ctx.inited = true;
    }

    return rc;
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::deinit()
{
    ecl_assert(m_ctx.inited);
    // TODO
    m_ctx.inited = false;
    return err::ok;
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::write(const uint8_t *data, size_t &count)
{
    ecl_assert(m_ctx.inited);

    m_ctx.state.clear();

    auto fn = [data](size_t data_offt, size_t blk_offt, size_t to_copy) {
        memcpy(m_ctx.block.buf + blk_offt, data + data_offt, to_copy);
        m_ctx.block.mint = false;
    };

    return traverse_data(count, fn);
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::read(uint8_t *data, size_t &count)
{
    ecl_assert(m_ctx.inited);

    m_ctx.state.clear();

    auto fn = [data](size_t data_offt, size_t blk_offt, size_t to_copy) {
        memcpy(data + data_offt, m_ctx.block.buf + blk_offt, to_copy);
    };

    return traverse_data(count, fn);
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::flush()
{
    ecl_assert(m_ctx.inited);

    m_ctx.state.clear();

    err rc;

    spi_dev::lock();
    gpio_cs::reset();

    rc = flush_block();

    gpio_cs::set();
    spi_dev::unlock();

    return rc;
}

// TODO: change off_t to int
template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::seek(off_t offset)
{
    ecl_assert(m_ctx.inited);
    m_ctx.state.clear();
    m_ctx.offt = offset;
    return err::ok;
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::tell(off_t &offt)
{
    ecl_assert(m_ctx.inited);
    m_ctx.state.clear();
    offt = m_ctx.offt;
    return err::ok;
}

template<class spi_dev, class gpio_cs>
constexpr size_t sdspi<spi_dev, gpio_cs>::get_block_length()
{
    return ctx_type::block_type::block_len;
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::get_info(sdspi_card_info &info)
{
    R1  r1;
    err rc;
    uint8_t cid[16];

    spi_dev::lock();
    gpio_cs::reset();

    rc = CMD10(r1);
    if (is_error(rc)) {
        return rc;
    }

    rc = receive_data(cid, sizeof(cid));
    if (is_error(rc)) {
        return rc;
    }

    gpio_cs::set();
    spi_dev::unlock();

    info.mid = cid[0];
    info.oid[0] = cid[1];
    info.oid[1] = cid[2];
    info.oid[2] = 0;

    memcpy(info.pnm, &cid[3], 5);
    info.pnm[5] = 0;

    info.prv_high = cid[8] >> 4;
    info.prv_high = cid[8] & 0xf;
    info.psn = (cid[9] << 24) | (cid[10] << 16) | (cid[11] << 8) | cid[12];
    info.mdt_year = 2000 + (((cid[13] & 0xf) << 4) | (cid[14] >> 4));
    info.mdt_month = cid[14] & 0xf;

    info.type_hc = m_ctx.hc;

    return err::ok;
}

template<class spi_dev, class gpio_cs>
const sdspi_state &sdspi<spi_dev, gpio_cs>::get_state()
{
    return m_ctx.state;
}

// Private methods -------------------------------------------------------------

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::spi_send(const uint8_t *buf, size_t size)
{
    spi_dev::set_buffers(buf, nullptr, size);
    // TODO: verify that all data was transferred
    auto rc = spi_dev::xfer(&size, nullptr);

    // Do not propagate exact bus error code to user.
    if (is_error(rc)) {
        rc = err::io;
    }

    return rc;
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::spi_receive(uint8_t *buf, size_t size)
{
    spi_dev::set_buffers(nullptr, buf, size);
    // TODO: verify that all data was transferred
    auto rc = spi_dev::xfer(nullptr, &size);

    // Do not propagate exact bus error code to user.
    if (is_error(rc)) {
        rc = err::io;
    }

    return rc;
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::spi_send_dummy(size_t size)
{
    spi_dev::set_buffers(size);
    // TODO: verify that all data was transferred
    auto rc = spi_dev::xfer(&size, nullptr);

    // Do not propagate exact bus error code to user.
    if (is_error(rc)) {
        rc = err::io;
    }

    return rc;
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::send_init()
{
    // Initialise card with >= 74 clocks on start
    size_t dummy_bytes = 80; // TODO: bytes or clock pulses?
    return spi_send_dummy(dummy_bytes);
}

template<class spi_dev, class gpio_cs>
template<typename R>
err sdspi<spi_dev, gpio_cs>::send_CMD(R &resp, uint8_t CMD_idx, const argument &arg, uint8_t crc)
{
    err rc;

    CMD_idx &= 0x3f; // First two bits are reserved TODO: comment
    CMD_idx |= 0x40;

    crc |= 0x1; // EOT flag

    // Init a transaction
    spi_send_dummy(1);

    // Command body
    const uint8_t to_send[] =
        { CMD_idx, arg[0], arg[1], arg[2], arg[3], crc };

    // Send HCS
    rc = spi_send(to_send, sizeof(to_send));
    if (is_error(rc)) {
        return rc;
    }

    // Retrieve a result
    return receive_response(resp);
}

//------------------------------------------------------------------------------

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::receive_response(R1 &r)
{
    uint8_t tries = 8;
    err rc;

    do {
        rc = spi_receive(&r.response, 1);
        if (is_error(rc)) {
            return rc;
        }
    } while (!r.received() && --tries);

    if (!r.received()) {
        m_ctx.state.set_err_r1_expired();
        rc = err::generic;
    } else if (!r.ok()) {
        // bit-to-bit compatibility with response field
        m_ctx.state.set_r1_flags(r.response);
        rc = err::generic;
    }

    return rc;
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::receive_response(R3 &r)
{
    err rc = receive_response(r.r1);
    if (is_error(rc)) {
        return rc;
    }

    return spi_receive((uint8_t *)&r.OCR, sizeof(r.OCR));
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::receive_data(uint8_t *buf, size_t size)
{
    // Data token that returned in case of success
    static constexpr uint8_t data_token    = 0xfe;

    uint8_t  token = 0;
    uint16_t crc;
    uint8_t  tries = 64;
    err      rc;

    do {
        rc = spi_receive(&token, sizeof(token));
        if (is_error(rc)) {
            return rc;
        }
    } while (token == 0xff && --tries);

    if (!tries) {
        // ecl::cout << "Timeout waiting for data token" << ecl::endl;
        m_ctx.state.set_err_recv_tok_expired();
        return err::generic;
    }

    // Data token received,
    if (token == data_token) {
        rc = spi_receive(buf, size);
        if (is_error(rc)) {
            return rc;
        }
    } else {
        m_ctx.state.set_recv_tok_flags(token);
        return err::generic;
    }

    return spi_receive((uint8_t *)&crc, sizeof(crc));
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::send_data(const uint8_t *buf, size_t size)
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
    err      rc;

    // Token
    rc = spi_send(&data_token, sizeof(data_token));
    if (is_error(rc)) {
        return rc;
    }

    // Data itself
    rc = spi_send(buf, size);
    if (is_error(rc)) {
        return rc;
    }

    // Dummy CRC
    rc = spi_send(&crc, sizeof(crc));
    if (is_error(rc)) {
        return rc;
    }

    // Wait for data response
    do {
        rc = spi_receive(&data_response, sizeof(data_response));
        if (is_error(rc)) {
            return rc;
        }
    } while (((data_response & mask) != 0x1) && --tries);

    if (!tries) {
        m_ctx.state.set_err_send_resp_expired();
        return err::generic;
    }

    // No error occur, only 4 lower bits matters
    if ((data_response & 0x0f) == accepted) {
        // Wait till card is busy
        do {
            rc = spi_receive(&data_response, sizeof(data_response));
            if (is_error(rc)) {
                return rc;
            }
        } while (data_response == 0x0);

        return rc;
    }

    if (data_response & crc_err) {
        m_ctx.state.set_err_crc();
    }
    if (data_response & write_err) {
        m_ctx.state.set_err_write();
    }

    return err::generic;
}

//------------------------------------------------------------------------------

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::CMD0(R1 &r)
{
    // TODO: comments
    constexpr uint8_t  CMD0_idx = 0;
    constexpr uint8_t  CMD0_crc = 0x95;
    constexpr argument arg      = {0};
    return send_CMD(r, CMD0_idx, arg, CMD0_crc);
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::CMD8(R7 &r)
{
    // TODO: comments
    constexpr uint8_t   CMD8_idx = 8;
    constexpr uint8_t   CMD8_crc = 0x87;
    constexpr argument  arg      = { 0, 0, 0x01, 0xaa };
    return send_CMD(r, CMD8_idx, arg, CMD8_crc);
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::CMD10(R1 &r)
{
    // TODO: comments
    constexpr uint8_t   CMD10_idx  = 10;
    constexpr argument  arg        = { 0 };
    return send_CMD(r, CMD10_idx, arg);
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::CMD16(R1 &r)
{
    // TODO: comments
    constexpr uint8_t  CMD16_idx = 16;
    constexpr uint32_t block_len = ctx_type::block_type::block_len;
    constexpr argument arg = {
        (uint8_t) (block_len >> 24),
        (uint8_t) (block_len >> 16),
        (uint8_t) (block_len >> 8),
        (uint8_t) (block_len),
    };
    return send_CMD(r, CMD16_idx, arg);
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::CMD17(R1 &r, uint32_t address)
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

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::CMD24(R1 &r, uint32_t address)
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

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::CMD55(R1 &r)
{
    // TODO: comments
    constexpr uint8_t CMD55_idx = 55;
    constexpr argument arg      = { 0, 0, 0, 0 };
    return send_CMD(r, CMD55_idx, arg);
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::CMD58(R3 &r)
{
    // TODO: comments
    constexpr uint8_t CMD58_idx = 58;
    constexpr argument arg      = { 0, 0, 0, 0 };
    return send_CMD(r, CMD58_idx, arg);
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::ACMD41(R1 &r, bool HCS)
{
    const uint8_t HCS_byte = HCS ? (1 << 6) : 0;

    err rc = CMD55(r);
    if (is_error(rc)) {
        return rc;
    }

    // TODO: comments
    constexpr uint8_t  CMD41_idx  = 41;
    const argument     arg        = { HCS_byte, 0, 0, 0 };
    return send_CMD(r, CMD41_idx, arg);
}

//------------------------------------------------------------------------------

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::open_card()
{
    err rc;
    sd_type type;

    rc = software_reset();
    if (is_error(rc)) {
        return rc;
    }

    rc = check_conditions();
    if (is_error(rc)) {
        return rc;
    }

    rc = init_process();
    if (is_error(rc)) {
        return rc;
    }

    rc = check_OCR(type);
    if (is_error(rc)) {
        return rc;
    } else if (type == sd_type::hc) {
        m_ctx.hc = true;
    } else if (type == sd_type::sc) {
        rc = set_block_length();
        if (is_error(rc)) {
            return rc;
        }
    } else {
        return err::generic;
    }

    rc = populate_block(m_ctx.offt);

    return rc;
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::software_reset()
{
    R1 r1;

    err rc = CMD0(r1);
    if (is_error(rc)) {
        return rc;
    }

    if (r1.response & R1::idle_state) {
        m_ctx.state.set_idle_state();
        return err::ok;
    }

    m_ctx.state.set_err_invalid_state();

    return err::generic;
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::check_conditions()
{
    R7 r7;

    err rc = CMD8(r7);
    if (is_error(rc)) {
        return rc;
    }

    uint32_t OCR = ecl::BE(r7.OCR);

    if (r7.r1.response & R1::idle_state) {
        m_ctx.state.set_idle_state();
        if ((OCR & 0xff) != 0xaa) {
            m_ctx.state.set_err_pattern_mismatch();
            return err::generic;
        }

        if (!(OCR & 0x100)) {
            // ecl::cout << "Voltage range not accepted" << ecl::endl;
            m_ctx.state.set_err_voltage_range();
            return err::generic;
        }

        return err::ok;
    }

    m_ctx.state.set_err_invalid_state();
    return err::generic;
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::init_process()
{
    //TODO: comments
    R1 r1;
    err rc;

    r1.response = 0x1;
    uint8_t tries = 255;

    while ((r1.response & R1::idle_state) && --tries) {
        rc = ACMD41(r1, true);

        if (is_error(rc)) {
            return rc;
        }
    }

    if (!tries) {
        m_ctx.state.set_err_init_expired();
        return err::generic;
    }

    return err::ok;
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::check_OCR(sd_type &type)
{
    R3 r3;
    CMD58(r3);
    err rc = err::ok;

    uint32_t OCR = ecl::BE(r3.OCR);

    // TODO: compare current profile and supported one!
    // Otherwise we can damage the card
    if (!r3.r1.response) {
        // Card sends data (as well as OCR) in BE format
        if (OCR & (1 << 31)) {
            ecl::cout << "Card is powered-up" << ecl::endl;
            if (OCR & (1 << 30)) {
                ecl::cout << "Card is High capacity" << ecl::endl;
                type = sd_type::hc;
            } else {
                ecl::cout << "Card is Standard capacity" << ecl::endl;
                type = sd_type::sc;
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

        return rc;
    }

    m_ctx.state.set_r1_flags(r3.r1.response);
    return err::generic;
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::set_block_length()
{
    R1 r1;

    err rc = CMD16(r1);
    if (is_error(rc)) {
        return rc;
    }

    if (r1.response & R1::idle_state) {
        m_ctx.state.set_err_invalid_state();
        rc = err::generic;
    }

    return rc;
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::populate_block(size_t new_block)
{
    R1 r1;
    off_t address = m_ctx.hc ? new_block : new_block * ctx_type::block_type::block_len;

    err rc = flush_block();
    if (is_error(rc)) {
        return rc;
    }

    rc = CMD17(r1, address);
    if (is_error(rc)) {
        return rc;
    }

    rc = receive_data(m_ctx.block.buf, ctx_type::block_type::block_len);
    if (is_error(rc)) {
        return rc;
    }

    m_ctx.block.origin = new_block;
    return rc;
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::flush_block()
{
    err rc;
    R1 r1;

    if (m_ctx.block.mint) {
        return err::ok;
    }

    off_t address = m_ctx.hc ? m_ctx.block.origin :
        m_ctx.block.origin * ctx_type::block_type::block_len;

    rc = CMD24(r1, address);
    if (is_error(rc)) {
        return rc;
    }

    rc = send_data(m_ctx.block.buf, ctx_type::block_type::block_len);

    if (is_error(rc)) {
        return rc;
    }

    m_ctx.block.mint = true;
    return rc;
}

template<class spi_dev, class gpio_cs>
err sdspi<spi_dev, gpio_cs>::traverse_data(
        size_t count,
        const std::function< void (size_t, size_t, size_t) > &fn
        )
{
    size_t left = count;
    err rc = err::ok;

    // Intended to be optimized in right shift, since
    // block length is constant and a power of two
    size_t blk_num = m_ctx.offt / ctx_type::block_type::block_len;
    size_t blk_offt = m_ctx.offt - blk_num  * ctx_type::block_type::block_len;
    size_t data_offt = 0;

    while (left) {
        if (blk_num  != m_ctx.block.origin) {
            spi_dev::lock();
            gpio_cs::reset();
            rc = populate_block(blk_num);
            gpio_cs::set();
            spi_dev::unlock();

            if (is_error(rc)) {
                return rc;
            }
        }

        size_t to_copy = std::min(ctx_type::block_type::block_len - blk_offt , left);
        // Copy data
        fn(data_offt, blk_offt, to_copy);

        // Advance to next free chunk
        data_offt += to_copy;
        left -= to_copy;

        // If needed, next iteration will populate cache again
        blk_num++;
        // Next copy will occur within a new block
        blk_offt  = 0;
    }

    m_ctx.offt += count;

    return rc;
}

//! @}

//! @}

} // namespace ecl

#endif // DEV_SDSPI_H
