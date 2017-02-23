#include <dev/hm10.hpp>

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

// Error code helper
// TODO: move it to 'utils' headers and protect with check of
// current test state (enabled or disabled)
static SimpleString StringFrom(ecl::err err)
{
    return SimpleString{ecl::err_to_str(err)};
}

//------------------------------------------------------------------------------
// HC10 block mode mocks

struct hc10_uart_mock
{
public:
    static ecl::err init()
    {
        mock("uart_mock").actualCall("init");
        return (ecl::err) mock("uart_mock").returnIntValueOrDefault((int)ecl::err::ok);
    }

    static void lock()
    {
        mock("uart_mock").actualCall("lock");
        m_locked = true;
    }

    static void unlock()
    {
        mock("uart_mock").actualCall("unlock");
        m_locked = false;
    }

    static ecl::err set_buffers(const uint8_t *tx, uint8_t *rx, size_t size)
    {
        mock("uart_mock").actualCall("set_buffers")
                .withParameter("size", size)
                .withStringParameter("tx", (const char*)tx)
                .withPointerParameter("rx", rx);

        return (ecl::err) mock("uart_mock").returnIntValueOrDefault((int)ecl::err::ok);
    }

    static ecl::err set_buffers(const uint8_t *tx, uint8_t *rx, size_t tx_sz, size_t rx_sz)
    {
        m_tx = (const char*)tx;
        m_rx = (char*)rx;
        m_rx_sz = rx_sz;
        m_tx_sz = tx_sz;

        mock("uart_mock").actualCall("set_buffers");

        return (ecl::err) mock("uart_mock").returnIntValueOrDefault((int)ecl::err::ok);
    }

    static ecl::err xfer(size_t *sent = nullptr, size_t *received = nullptr,
                        std::chrono::milliseconds ms = std::chrono::milliseconds::max())
    {
        (void) ms;

        size_t recv_sz = 0;

        mock("uart_mock").actualCall("xfer")
                .withOutputParameter("rx_sz", &recv_sz)
                .withOutputParameter("rx", m_rx)
                .withStringParameter("tx", m_tx);

        // This check is intentionally placed here.
        // During invalid test cases, incoming buffer can be larger than provided buffer.
        // In a wild, platform driver must not overflow provided buffer.
        // But in tests it is hard to step into this, because output buffers are often
        // arbitrary in some tests.
        CHECK_TRUE(recv_sz <= m_rx_sz);

        // Assume all data is sent
        if (sent) {
            *sent = m_tx_sz;
        }

        if (received) {
            *received = std::min(recv_sz, m_rx_sz);
        }

        return (ecl::err) mock("uart_mock").returnIntValueOrDefault((int)ecl::err::ok);
    }

    struct platform_handle
    {
        static ecl::err enable_listen_mode()
        {
            return ecl::err::ok;
        }

        static ecl::err disable_listen_mode()
        {
            return ecl::err::ok;
        }
    };

    static void cleanup()
    {
        m_locked = false;
        m_tx = nullptr;
        m_rx = nullptr;
        m_rx_sz = 0;
        m_tx_sz = 0;
    }

    static const char   *m_tx;
    static char         *m_rx;
    static size_t       m_rx_sz; // Supplied buffer size
    static size_t       m_tx_sz;
    static bool         m_locked;
};

const char *hc10_uart_mock::m_tx;
char *hc10_uart_mock::m_rx;
size_t hc10_uart_mock::m_rx_sz;
size_t hc10_uart_mock::m_tx_sz;
bool hc10_uart_mock::m_locked;

//------------------------------------------------------------------------------

TEST_GROUP(hm10_sync_mode)
{
    void setup()
    {

    }

    void teardown()
    {
        hc10_uart_mock::cleanup();
        mock().clear();
    }
};

//------------------------------------------------------------------------------
// HC-10 common mode tests

using hm10_sync = ecl::hm10_sync<hc10_uart_mock>;

TEST(hm10_sync_mode, init)
{
    const char valid_resp[] = "OK";

    mock("uart_mock").strictOrder();

    mock("uart_mock").expectOneCall("init");
    mock("uart_mock").expectOneCall("lock");

    // Responses are not null-terminated (TODO: double check this)
    size_t valid_sz = sizeof(valid_resp) - 1;

    // Query module
    mock("uart_mock").expectOneCall("set_buffers");
    mock("uart_mock").expectOneCall("xfer")
            .withOutputParameterReturning("rx", valid_resp, sizeof(valid_resp) - 1)
            .withOutputParameterReturning("rx_sz", &valid_sz, sizeof(valid_sz))
            .withStringParameter("tx", "AT");

    auto rc = hm10_sync::init();

    CHECK_EQUAL(ecl::err::ok, rc);

    mock().checkExpectations();
}

TEST(hm10_sync_mode, init_invalid_resp)
{
    const char invalid_resp[] = "BZ";
    size_t inv_sz = sizeof(invalid_resp) - 1;

    mock("uart_mock").expectOneCall("init");
    mock("uart_mock").expectOneCall("lock");

    // HM10 will try to query module 3 times before failing dur to invalid response.

    mock("uart_mock").expectNCalls(3, "set_buffers");
    mock("uart_mock").expectNCalls(3, "xfer")
            .withOutputParameterReturning("rx", invalid_resp, sizeof(invalid_resp))
            .withOutputParameterReturning("rx_sz", &inv_sz, sizeof(inv_sz))
            .withStringParameter("tx", "AT");

    auto rc = hm10_sync::init();

    CHECK_EQUAL(ecl::err::inval, rc);

    mock().checkExpectations();
}

TEST(hm10_sync_mode, init_timeout_valid_resp)
{
    // Even if timeout is hit, data from RX stream should be processed.

    const char valid_resp[] = "OK";

    mock("uart_mock").strictOrder();

    mock("uart_mock").expectOneCall("init");
    mock("uart_mock").expectOneCall("lock");

    // Responses are not null-terminated (TODO: double check this)
    size_t valid_sz = sizeof(valid_resp) - 1;

    // Query module
    mock("uart_mock").expectOneCall("set_buffers");
    mock("uart_mock").expectOneCall("xfer")
            .withOutputParameterReturning("rx", valid_resp, sizeof(valid_resp) - 1)
            .withOutputParameterReturning("rx_sz", &valid_sz, sizeof(valid_sz))
            .withStringParameter("tx", "AT")
            .andReturnValue((int)ecl::err::timedout);

    auto rc = hm10_sync::init();

    CHECK_EQUAL(ecl::err::ok, rc);

    mock().checkExpectations();
}

TEST(hm10_sync_mode, init_timeout_no_resp)
{
    // With no bytes written to RX, driver must return timeout in case if such
    // was returned from UART.
    size_t resp_sz = 0;

    mock("uart_mock").expectOneCall("init");
    mock("uart_mock").expectOneCall("lock");

    // Query module
    mock("uart_mock").expectNCalls(3, "set_buffers");
    mock("uart_mock").expectNCalls(3, "xfer")
            .withOutputParameterReturning("rx", "", 0)
            .withOutputParameterReturning("rx_sz", &resp_sz, sizeof(resp_sz))
            .withStringParameter("tx", "AT")
            .andReturnValue((int)ecl::err::timedout);

    auto rc = hm10_sync::init();

    CHECK_EQUAL(ecl::err::timedout, rc);

    mock().checkExpectations();
}

TEST(hm10_sync_mode, send)
{

}


int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
