/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief HM-10 BT module drivers.
//! Header must expose both synchronous (simple but not so functional) and
//! asynchronious versions of the driver.
//! At this moment, only synchronous version is implemented.

#ifndef DEV_BT_HM10_HPP_
#define DEV_BT_HM10_HPP_

#include <ecl/err.hpp>
#include <ecl/assert.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <algorithm>
#include <chrono>

namespace ecl
{

//! HM10 synchronous BT driver.
//! \details This HM10 driver is simple and low-functional API to send and receive
//! data from BT module. It lacks connection status detection and does not escape
//! potential AT commands found in payload (yet). No flow control is used
//! whatsoever.
//! External protocol is required to control whether other end is ready to
//! receive or send a data. Great advantage of this particular driver is its API.
//! Straitforward and blocking it can be used for simple demo implementations.
//! \note Driver _is_not_ a thread safe. Using driver routine from ISR context
//! will lead to undefined behaviour.
//! \tparam UART bus to work with.
template<typename Uart>
class hm10_sync
{
public:
    //! Defines internal timeout in milliseconds during which module has to
    //! respond to commands.
    static constexpr auto cmd_timeout() { return std::chrono::milliseconds(1000); }

    //! Initializes BT module.
    //! \return Status of operation.
    //! \retval err::inval      Module responded with invalid data.
    //! \retval err::timedout   Module did not respond at all.
    //! \retval err::ok         Module is ready to work.
    static err init();

    //! Sends data to the module.
    //! \warning It is possible that no one has connected to the device yet.
    //! \warning Data that is sent to unconnected module will be lost.
    //! \details Call blocks until all data will be sent.
    //! \param[in]      buf Buffer to send.
    //! \param[in,out]  sz  Size of buffer as input, bytes written as output.
    //! \return Status of operation.
    //! \retval err::ok         Data sent successfully.
    static err send(const uint8_t *buf, size_t &sz);

    //! Receives data from the module with given timeout.
    //! \param[out]     buf Buffer to store incoming data.
    //! \param[in,out]  sz  Size of the buffer as input, bytes effectively written
    //!                     as output.
    //! \param[in]      ms  Optional. Defines overall time to wait for incoming data.
    //!                     Defaults to infinite time.
    //! \return Status of operation.
    //! \retval err::timedout   Buffer was not filled completely before timeout hit.
    //!                         It is still possible that some data was transferred.
    //!                         Check \ref sz to understand how many bytes are there.
    //! \retval err::ok         Whole buffer was populated before timeout reached.
    static err recv(uint8_t *buf, size_t &sz,
                    std::chrono::milliseconds ms = std::chrono::milliseconds::max());

private:
    //! Sends command to the module and compares response with given expectatations.
    //! \details Commands are ASCII-based strings, started with "AT".
    //! Certain and unknown (for now) delay is used as a command terminator.
    //! \param[in] cmd      Command to send.
    //! \param[in] cmd_sz   Command size in bytes.
    //! \param[in] exp_resp Expected response.
    //! \param[in] exp_sz   Size of expected response.
    //! \param[in] ms       Time to wait for response.
    //! \return Status of operation.
    //! \retval err::timedout   Response is not received within given timeout.
    //! \retval err::inval      Response is received but it is invalid.
    //! \retval err::ok         Command succeed.
    //! \todo Use uint8_t instead of char*.
    static err send_cmd(const char *cmd, size_t cmd_sz, const char *exp_resp, size_t exp_sz,
                    std::chrono::milliseconds ms = cmd_timeout());

    //! Sends/receives data to the module with given timeout.
    //! \param[in]      tx      Buffer to send.
    //! \param[out]     rx      Buffer to store data to.
    //! \param[in,out]  tx_sz   Size of the transmit buffer as input and
    //!                         amount of bytes actually transmitted as output.
    //! \param[in,out]  rx_sz   Size of the receive buffer as input and
    //!                         amount of bytes actually written to the buffer as output.
    //! \param[in] ms           Time to wait for response.
    //! \return Status of operation.
    //! \retval err::timedout   Response is not received within given timeout.
    //! \retval err::ok         Command succeed.
    static err xfer(const uint8_t *tx, uint8_t *rx, size_t &tx_sz, size_t &rx_sz,
                    std::chrono::milliseconds ms);
};

//------------------------------------------------------------------------------

template<typename Uart>
err hm10_sync<Uart>::init()
{
    constexpr char at_cmd[]             = "AT";
    constexpr char at_cmd_resp[]        = "OK";

    Uart::init();
    Uart::lock();

    // Exclude null-character
    err rc;

    // Try few times, since there is possible module glitches due to power reset
    for (int cnt = 0;
            cnt < 3 && is_error(rc = send_cmd(at_cmd, sizeof(at_cmd) - 1,
                            at_cmd_resp, sizeof(at_cmd_resp) - 1)); cnt++)
                            { }

    if (is_error(rc)) {
        return rc;
    }

    return rc;
}

template<typename Uart>
err hm10_sync<Uart>::send(const uint8_t *buf, size_t &sz)
{
    size_t dummy_sz = 0;
    return xfer(buf, nullptr, sz, dummy_sz, std::chrono::milliseconds(1000));
}

template<typename Uart>
err hm10_sync<Uart>::recv(uint8_t *buf, size_t &sz, std::chrono::milliseconds ms)
{
    size_t dummy_sz = 0;
    return xfer(nullptr, buf, dummy_sz, sz, ms);
}

//------------------------------------------------------------------------------

template<typename Uart>
err hm10_sync<Uart>::send_cmd(const char *cmd, size_t cmd_sz, const char *exp_resp,
                                    size_t exp_sz, std::chrono::milliseconds ms)
{
    char resp[32] = {0};

    // TODO: provide better buffer management.
    ecl_assert(sizeof(resp) > exp_sz);

    size_t resp_sz = exp_sz;

    auto rc = xfer(reinterpret_cast<const uint8_t*>(cmd),
                   reinterpret_cast<uint8_t*>(resp),
                   cmd_sz, resp_sz, ms);

    // If no data is written to the TX buffer, then there is no need to proceed
    // further.
    if (!resp_sz && is_error(rc)) {
        return rc;
    }

    // Check response received. Note that output _likely_  is not null-terminated.
    // Even if xfer failed due to timeout, incoming data is still useful.
    if (strncmp(resp, exp_resp, resp_sz)) {
        // Unexpected response or no data is received for some reason.
        // TODO: what if there will be payload bytes after the command response?
        return err::inval;
    }

    return err::ok;
}

template<typename Uart>
err hm10_sync<Uart>::xfer(const uint8_t *tx, uint8_t *rx, size_t &tx_sz, size_t &rx_sz,
                                std::chrono::milliseconds ms)
{
    Uart::set_buffers(tx, rx, tx_sz, rx_sz);

    // Really matters only if RX is present
    if (rx) {
        // Count every byte received
        Uart::platform_handle::enable_listen_mode();
    }

    // Timeout is added to cover case when module is not responding.
    auto rc = Uart::xfer(&tx_sz, &rx_sz, ms);

    if (rx) {
        Uart::platform_handle::disable_listen_mode();
    }

    return rc;
}

} // namespace ecl

#endif // DEV_BT_HM10_HPP_
