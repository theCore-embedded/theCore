#ifndef DEV_BT_HC10_HPP_
#define DEV_BT_HC10_HPP_

#include <ecl/err.hpp>
#include <ecl/assert.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <algorithm>
#include <chrono>

namespace ecl
{

//! HC10 BT driver.
//! \tparam Uart bus to work with.
template<typename Uart>
class hc10_sync
{
public:
    // Inits BT driver
    static err init();

    // Waits for connection
    static err wait_connect(std::chrono::milliseconds ms = std::chrono::milliseconds::max());

    // Sends data
    static err send(const uint8_t *buf, size_t sz);

private:
    // Blocking send-recieve command with given timeout.
    static err send_cmd(const char *cmd, size_t cmd_sz, const char *exp_resp, size_t exp_sz,
                    std::chrono::milliseconds ms = std::chrono::milliseconds(1000));

    // Blocking xfer.
    static err xfer(const uint8_t *tx, uint8_t *rx, size_t &tx_sz, size_t &rx_sz,
                    std::chrono::milliseconds ms);
};

//------------------------------------------------------------------------------

template<typename Uart>
err hc10_sync<Uart>::init()
{
    constexpr char at_cmd[]             = "AT";
    constexpr char at_cmd_resp[]        = "OK";
    constexpr char at_noti_cmd[]        = "AT+NOTI1";
    constexpr char at_noti_resp[]       = "OK+Set:1";

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

    // Set notification mode. When master will connect, module will respond with
    // OK+CONN
    rc = send_cmd(at_noti_cmd, sizeof(at_noti_cmd) - 1,
                        at_noti_resp, sizeof(at_noti_resp) - 1);

    return rc;
}

template<typename Uart>
err hc10_sync<Uart>::wait_connect(std::chrono::milliseconds ms)
{
    // TODO: assert if not inited

    // Wait until somebody will connect
    return send_cmd(nullptr, 0, "OK+CONN", sizeof("OK+CONN") - 1, ms);
}

template<typename Uart>
err hc10_sync<Uart>::send(const uint8_t *buf, size_t sz)
{
    size_t dummy_sz = 0;
    return xfer(buf, nullptr, sz, dummy_sz, std::chrono::milliseconds(1000));
}

//------------------------------------------------------------------------------

template<typename Uart>
err hc10_sync<Uart>::send_cmd(const char *cmd, size_t cmd_sz, const char *exp_resp,
                                    size_t exp_sz, std::chrono::milliseconds ms)
{
    char resp[32] = {0};

    // TODO: provide better buffer management.
    ecl_assert(sizeof(resp) > exp_sz);

    size_t resp_sz = exp_sz;

    auto rc = xfer(reinterpret_cast<const uint8_t*>(cmd),
                   reinterpret_cast<uint8_t*>(resp),
                   cmd_sz, resp_sz, ms);

    // Unexpected error
    if (rc != err::timedout && is_error(rc)) {
        return rc;
    }

    // Check if some response is received befor timeout. Note that output _likely_
    // is not null-terminated.
    if (!resp_sz || strncmp(resp, exp_resp, resp_sz)) {
        // Unexpected response or no data is received for some reason.
        // TODO: what if there will be payload bytes after the command response?
        return err::inval;
    }

    return err::ok;
}

template<typename Uart>
err hc10_sync<Uart>::xfer(const uint8_t *tx, uint8_t *rx, size_t &tx_sz, size_t &rx_sz,
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

#endif // DEV_BT_HC10_HPP_
