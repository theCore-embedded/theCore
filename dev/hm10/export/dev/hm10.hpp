/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief HM-10 BT module drivers.
//! \todo Header must expose both synchronous (simple but not so functional) and
//! asynchronous versions of the driver.
//! At this moment, only synchronous version is implemented.

#ifndef DEV_BT_HM10_HPP_
#define DEV_BT_HM10_HPP_

#include <ecl/err.hpp>
#include <ecl/assert.h>
#include <dev/serial.hpp>
#include <common/execution.hpp>

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <algorithm>
#include <inttypes.h>

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
//! \tparam Uart instance of the UART platform driver.
//! \tparam Serial instance attached to the UART bus to work with.
//!         Provided for mock injection in unit tests.
template<class Uart, class Serial = ecl::serial<Uart>>
class hm10_sync
{
public:
    //! Defines internal timeout in milliseconds during which module has to
    //! respond to commands.
    static constexpr auto cmd_timeout() { return std::chrono::milliseconds(1000); }

    //! Defines internal timeout in milliseconds between retries to send or
    //! receive data
    static constexpr auto retry_timeout() { return std::chrono::milliseconds(50); }

    //! Initializes BT module.
    //! \return Status of operation.
    //! \retval err::inval      Module responded with invalid data.
    //! \retval err::timedout   Module did not respond at all.
    //! \retval err::ok         Module is ready to work.
    static err init();

    //! Sets pin code for connecting to the bluetooth device.
    //! \details Sends AT+PASS  command.
    //! \param[in] pin PIN to set. Must be in a range of 000000-999999
    //! \return Status of operation.
    //! \retval err::inval      Module responded with invalid data.
    //! \retval err::timedout   Module did not respond at all.
    //! \retval err::ok         New pin code is set.
    static err set_pin(uint32_t pin);

    //! Gets pin code from the device.
    //! \details Sends AT+PASS? command.
    //! \warning In case of error, content of pin argument is undefined.
    //! \param[out] pin PIN set for current device.
    //! \retval err::inval      Module responded with invalid data.
    //! \retval err::timedout   Module did not respond at all.
    //! \retval err::ok         Pin retrieved successfully.
    static err get_pin(uint32_t &pin);

    //! Sets immediate mode of operation.
    //! \pre Module in the master mode.
    //! \details Sends AT+IMME command.
    //! Immediate mode controls how module behaves after power reset.
    //! If immediate mode is set, then module will be listening to commands
    //! and will not start any connection attempt..
    //! Otherwise, module will operate immediately, (assuming a previous
    //! connection has been applied)
    //! \warning Immediate mode setting is preserved between module power cycles.
    //! \param[in] state New state of immediate mode: `true` - enables immediate
    //!                  mode, `false` - disables it.
    //! \retval err::inval      Module responded with invalid data.
    //! \retval err::timedout   Module did not respond at all.
    //! \retval err::ok         Immediate mode state is updated.
    static err set_immediate(bool state = true);

    //! Gets immediate mode state.
    //! \pre Module in the master mode.
    //! \details Sends AT+IMME? command.
    //! \sa set_immediate()
    //! \param[out] state Current state of immediate mode:
    //!                   `true` - immediate mode is enabled.
    //!                   `false` - immediate mode is disabled.
    //! \retval err::inval      Module responded with invalid data.
    //! \retval err::timedout   Module did not respond at all.
    //! \retval err::ok         Immediate mode state retrieved succesfully.
    static err get_immediate(bool &state);

    //! Send disconnect command.
    //! \details sends AT command.
    //! HM-10 will disconnect from any device.
    //! \retval err::inval      Module responded with invalid data.
    //! \retval err::timedout   Module did not respond at all.
    //! \retval err::ok         Module has been successfully reset.
    static err disconnect();

    //! Manually Starts communication.
    //! \pre Module is in immediate mode and acts like a master.
    //! \sa set_immediate()
    //! \details Sends AT+START command
    //! \retval err::inval      Module responded with invalid data.
    //! \retval err::timedout   Module did not respond at all.
    //! \retval err::ok         Module now can send and receive data.
   static err start();

    //! Sends data to the module.
    //! \warning It is possible that no one has connected to the device yet.
    //! \warning Data that is sent to unconnected module will be lost.
    //! \details Call blocks until all data will be sent.
    //! \param[in]      buf Buffer to send.
    //! \param[in,out]  sz  Size of buffer as input, bytes written as output.
    //! \return Status of operation.
    //! \retval err::ok         Data sent successfully.
    static err data_send(const uint8_t *buf, size_t &sz);

    //! Receives data from the module with given timeout.
    //! \param[out]     buf Buffer to store incoming data.
    //! \param[in,out]  sz  Size of the buffer as input, bytes effectively written
    //!                     as output.
    //! \param[in]      ms  Optional. Defines overall time to wait for incoming data.
    //!                     Defaults to infinite time.
    //! \return Status of operation.
    //! \retval err::timedout   Buffer was not filled completely before timeout hit.
    //!                         It is still possible that some data was transferred.
    //!                         Check \param sz to understand how many bytes are there.
    //! \retval err::ok         Whole buffer was populated before timeout reached.
    static err data_recv(uint8_t *buf, size_t &sz,
                    std::chrono::milliseconds ms = std::chrono::milliseconds::max());

private:
    //! Sends command to the module and compares response with given expectatations.
    //! \details Commands are ASCII-based strings, started with "AT".
    //! Certain and unknown (for now) delay is used as a command terminator.
    //! \param[in] cmd      Command to send.
    //! \param[in] cmd_sz   Command length in bytes (excluding NUL-character)
    //! \param[in] exp_resp Expected response.
    //! \param[in] exp_sz   Size of expected response (1 byte must be reserved for NUL)
    //! \param[in] ms       Time to wait for response.
    //! \return Status of operation.
    //! \retval err::timedout   Response is not received within given timeout.
    //! \retval err::inval      Response is received but it is invalid.
    //! \retval err::ok         Command succeed.
    //! \todo Use uint8_t instead of char*.
    static err send_cmd_checked(const char *cmd, size_t cmd_sz,
                    const char *exp_resp, size_t exp_sz,
                    std::chrono::milliseconds ms = cmd_timeout());

    //! Sends command and retrieves responce without validating it.
    //! \details Commands are ASCII-based strings, started with "AT".
    //! Certain and unknown (for now) delay is used as a command terminator.
    //! \param[in]      cmd     Command to send.
    //! \param[in]      cmd_sz  Command length in bytes (excluding NUL-character)
    //! \param[out]     resp    Buffer for response.
    //! \param[in,out]  resp_sz Size of response buffer on entry (1 byte must
    //!                         be reserved for NUL), actual responce bytes
    //!                         on exit (including NUL)
    //! \param[in]      ms      Time to wait for response.
    //! \return Status of operation.
    //! \retval err::timedout   Response is not received within given timeout.
    //! \retval err::ok         Command succeed.
    //! \todo Use uint8_t instead of char*.
    static err send_cmd(const char *cmd, size_t cmd_sz, char *resp, size_t &resp_sz,
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

template<class Uart, class Serial>
err hm10_sync<Uart, Serial>::init()
{
    Serial::init();
    Serial::nonblock(true);

    err rc;

    // Try few times, since there is possible module glitches due to power reset
    // Disconnect method sends 'AT' and that's fine.
    for (int cnt = 0; cnt < 3 && is_error(rc = disconnect()); cnt++) { }

    if (is_error(rc)) {
        return rc;
    }

    return rc;
}

template<class Uart, class Serial>
err hm10_sync<Uart, Serial>::set_pin(uint32_t pin)
{
    ecl_assert(pin <= 999999);

    // 14 chars = 7 chars for 'AT+PASS' string + 6 chars PIN string + NUL-char.
    // Rounded for convenience.
    char pass_cmd[16];
    // 14 chars = 7 chars for 'OK+Set:' string + 6 chars PIN string + NUL-char.
    // Rounded for convenience.
    constexpr auto reps_sz = 14;
    char pass_cmd_resp[16];

    int n = snprintf(pass_cmd, sizeof(pass_cmd), "%s%6" PRIu32 "", "AT+PASS", pin);
    int nr = snprintf(pass_cmd_resp, sizeof(pass_cmd_resp), "%s%6" PRIu32 "", "OK+Set:", pin);

    ecl_assert(n > 0 && static_cast<unsigned>(n) < sizeof(pass_cmd) - 1);
    ecl_assert(nr > 0 && static_cast<unsigned>(nr) == reps_sz - 1);
    return send_cmd_checked(pass_cmd, n, pass_cmd_resp, reps_sz);
}

template<class Uart, class Serial>
err hm10_sync<Uart, Serial>::get_pin(uint32_t &pin)
{
    constexpr char pass_cmd[] = "AT+PASS?";
    constexpr char resp_header[] = "OK+PASS:";

    // 8 chars for 'OK+PASS:' string + 6 chars PIN string + NUL-char = 15
    // Rounded for convenience.
    char pass_resp[16];
    char *pass_ptr = pass_resp;
    constexpr size_t exp_sz = 15;
    size_t ret_sz = exp_sz;

    err rc = send_cmd(pass_cmd, sizeof(pass_cmd) - 1, pass_ptr, ret_sz);

    if (is_error(rc)) {
        return rc;
    }

    // Responce must be in format OK+PASS:xxxxxx where 'xxxxxx' is a 6 digit
    // pin code.

    // Skip NUL at the end
    if (strncmp(pass_ptr, resp_header, sizeof(resp_header) - 1)) {
        return err::inval;
    }

    // Move to the digits part
    pass_ptr += sizeof(resp_header) - 1;
    char *endptr;
    auto val = strtol(pass_ptr, &endptr, 10);

    if ((endptr - pass_resp != exp_sz - 1) || val > 999999) {
        // Presumably, some data is there and it is not a pin.
        return err::inval;
    }

    pin = val;

    return rc;
}

template<class Uart, class Serial>
err hm10_sync<Uart, Serial>::set_immediate(bool state)
{
    // 8 chars for 'AT+IMME:' string + 1 char state + NUL-char = 10 chars
    // Rounded for convenience.
    constexpr auto imme_len = sizeof("AT+IMME ") - 1;
    char imme_cmd[16] = "AT+IMME ";
    // Place state flag at the end of a cmd string
    imme_cmd[sizeof("AT+IMME") - 1] = state ? '1' : '0';

    // 7 chars for 'OK+Set:' string + 1 char state flag + NUL-char = 9 chars
    // Rounded for convenience.
    constexpr auto reps_sz = sizeof("OK+Set: ");
    char resp[16] = "OK+Set: ";
    // Place expected state flag at the end of a cmd response string
    resp[sizeof("AT+IMME") - 1] = state ? '1' : '0';

    return send_cmd_checked(imme_cmd, imme_len, resp, reps_sz);
}

template<class Uart, class Serial>
err hm10_sync<Uart, Serial>::get_immediate(bool &state)
{
    constexpr char imme_cmd[] = "AT+IMME?";
    constexpr char resp_header[] = "OK+Get:";

    // 7 chars for 'OK+Get:' string + 1 char state flag + NUL-char = 9
    constexpr size_t exp_sz = sizeof("OK+Get: ");
    size_t ret_sz = exp_sz;
    // Size rounded for convenience.
    char resp[16];
    char *resp_ptr = resp;

    err rc = send_cmd(imme_cmd, sizeof(imme_cmd) - 1, resp_ptr, ret_sz);

    if (is_error(rc) || ret_sz != exp_sz) {
        return rc;
    }

    // Responce must be in format OK+Get:x where 'x' is a 1 digit mode state.

    // Skip NUL at the end
    if (strncmp(resp_ptr, resp_header, sizeof(resp_header) - 1)) {
        return err::inval;
    }

    resp_ptr += sizeof(resp_header) - 1;

    // Last character must be '0' or '1'
    switch (*resp_ptr) {
    case '1':
        state = true;
        break;
    case '0':
        state = false;
        break;
    default:
        rc = err::inval;
        break;
    }

    return rc;
}

template<class Uart, class Serial>
err hm10_sync<Uart, Serial>::disconnect()
{
    constexpr char at_cmd[] = "AT";
    constexpr char resp[]   = "OK";

    return send_cmd_checked(at_cmd, sizeof(at_cmd) - 1, resp, sizeof(resp));
}

template<class Uart, class Serial>
err hm10_sync<Uart, Serial>::data_send(const uint8_t *buf, size_t &sz)
{
    size_t dummy_sz = 0;
    return xfer(buf, nullptr, sz, dummy_sz, std::chrono::milliseconds(1000));
}

template<class Uart, class Serial>
err hm10_sync<Uart, Serial>::data_recv(uint8_t *buf, size_t &sz, std::chrono::milliseconds ms)
{
    size_t dummy_sz = 0;
    return xfer(nullptr, buf, dummy_sz, sz, ms);
}

//------------------------------------------------------------------------------

template<class Uart, class Serial>
err hm10_sync<Uart, Serial>::send_cmd_checked(const char *cmd, size_t cmd_sz,
                                    const char *exp_resp, size_t exp_sz,
                                    std::chrono::milliseconds ms)
{
    char resp[32];
    // TODO: provide better buffer management.
    ecl_assert(exp_sz < sizeof(resp));

    size_t resp_sz = exp_sz;
    auto rc = send_cmd(cmd, cmd_sz, resp, resp_sz, ms);

    resp_sz--; // Do not count NUL character

    // If no data is written to the RX buffer, then there is no need to proceed
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

    return rc;
}

template<class Uart, class Serial>
err hm10_sync<Uart, Serial>::send_cmd(const char *cmd, size_t cmd_sz,
                                char *resp, size_t &resp_sz,
                                std::chrono::milliseconds ms)
{
    size_t resp_len = resp_sz - 1; // Reserve one byte for NUL-character
    auto rc = xfer(reinterpret_cast<const uint8_t*>(cmd),
                   reinterpret_cast<uint8_t*>(resp),
                   cmd_sz, resp_len, ms);

    // NUL-terminate returned data
    resp[resp_len] = 0;

    resp_sz = resp_len + 1;

    return rc;
}

template<class Uart, class Serial>
err hm10_sync<Uart, Serial>::xfer(const uint8_t *tx, uint8_t *rx, size_t &tx_sz, size_t &rx_sz,
                                std::chrono::milliseconds ms)
{
    err rc = err::ok;

    auto do_xfer = [&ms](auto fn, auto *buf, auto &sz) {
        using namespace std; // For suffixes

        err rc = err::timedout;
        size_t ret_sz = 0;
        size_t left = sz;
        do {
            ret_sz = left;
            rc = fn(buf, ret_sz);

            // Probably that not all data was read/sent.
            left -= ret_sz;
            buf += ret_sz;

            // Do not wait for retry too long
            auto retry = ms > retry_timeout() ? retry_timeout() : ms;

            // Do not stop if not all data received
            if (rc == err::wouldblock || left) {
                rc = err::timedout; // Better error code for hm10 user
                ecl::wait_for(retry);
            } else { // TODO: rc == err::again?
                break;
            }

            ms = ms > retry_timeout() ? ms - retry_timeout() : 0ms;
        } while (ms.count());

        sz = sz - left;
        return rc;
    };

    if (tx) {
        rc = do_xfer(Serial::send_buf, tx, tx_sz);
    }

    if (rx) {
        rc = do_xfer(Serial::recv_buf, rx, rx_sz);
    }

    return rc;
}

} // namespace ecl

#endif // DEV_BT_HM10_HPP_
