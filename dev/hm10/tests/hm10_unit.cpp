/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <dev/hm10.hpp>

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

//------------------------------------------------------------------------------
// HM10 block mode mocks

struct hm10_serial_mock
{
    static bool generate_wouldblock;

    static ecl::err init()
    {
        mock("serial_mock").actualCall("init");
        return (ecl::err) mock("serial_mock").returnIntValueOrDefault((int)ecl::err::ok);
    }

    static void nonblock(bool state)
    {
        (void)state;
        mock("serial_mock").actualCall("nonblock");
    }


    static ecl::err send_buf(const uint8_t *buf, size_t &sz)
    {
        static char inter_buf[1024];

        // This check is intentionally placed here.
        // Buffer must hold all data that hm10 driver sends.
        CHECK_TRUE(sz < sizeof(inter_buf) - 1);

        memcpy(inter_buf, buf, sz);
        inter_buf[sz] = 0;

        mock("serial_mock").actualCall("send_buf")
                .withStringParameter("tx", inter_buf);

        return (ecl::err) mock("serial_mock").returnIntValueOrDefault((int)ecl::err::ok);
    }

    static ecl::err recv_buf(uint8_t *buf, size_t &sz)
    {
        if (generate_wouldblock) {
            sz = 0;
            return ecl::err::wouldblock;
        }

        size_t recv_sz = 0;

        static char inter_buf[1024];
        memset(inter_buf, 0, sizeof(inter_buf));

        auto &mk = mock("serial_mock").actualCall("recv_buf")
                .withOutputParameter("rx_sz", &recv_sz);

        // This check is intentionally placed here.
        // Tests must not supply more data than buffer can hold.
        // It reality, serial driver would not write more data than incoming
        // buf can hold.
        CHECK_TRUE(recv_sz < sizeof(inter_buf) - 1);

        // Write data into the intermediate buffer.
        mk.withOutputParameter("rx", inter_buf);

        // Copy data from the intermediate buffer into the output parameter
        sz = std::min(recv_sz, sz);
        strncpy((char*)buf, inter_buf, sz);

        return (ecl::err) mock("serial_mock").returnIntValueOrDefault((int)ecl::err::ok);
    }
};

bool hm10_serial_mock::generate_wouldblock;

//------------------------------------------------------------------------------

TEST_GROUP(hm10_sync_mode)
{
    void setup()
    {

    }

    void teardown()
    {
        hm10_serial_mock::generate_wouldblock = false;
        mock().clear();
    }
};

//------------------------------------------------------------------------------
// HM-10 common mode tests

using hm10_sync = ecl::hm10_sync<hm10_serial_mock>;

// Reused in couple of different test groups
static void do_valid_init()
{
    const char valid_resp[] = "OK";

    mock("serial_mock").strictOrder();

    mock("serial_mock").expectOneCall("init");
    mock("serial_mock").expectOneCall("nonblock");

    // Responses are not null-terminated (TODO: double check this)
    size_t valid_sz = sizeof(valid_resp) - 1;

    // Query module
    mock("serial_mock").expectOneCall("send_buf")
            .withStringParameter("tx", "AT");

    mock("serial_mock").expectOneCall("recv_buf")
            .withOutputParameterReturning("rx", valid_resp, valid_sz)
            .withOutputParameterReturning("rx_sz", &valid_sz, sizeof(valid_sz));

    auto rc = hm10_sync::init();

    CHECK_EQUAL(ecl::err::ok, rc);

    mock().checkExpectations();
}

TEST(hm10_sync_mode, init)
{
    do_valid_init();
}

TEST(hm10_sync_mode, init_partial_resp)
{
    const char valid_resp[] = "OK";
    const char *rx = valid_resp;

    // Send one byte at time
    size_t valid_sz = 1;

    mock("serial_mock").expectOneCall("init");
    mock("serial_mock").expectOneCall("nonblock");

    // Query module
    mock("serial_mock").expectOneCall("send_buf")
            .withStringParameter("tx", "AT");

    mock("serial_mock").expectOneCall("recv_buf")
            .withOutputParameterReturning("rx", rx++, valid_sz)
            .withOutputParameterReturning("rx_sz", &valid_sz, sizeof(valid_sz));
    mock("serial_mock").expectOneCall("recv_buf")
            .withOutputParameterReturning("rx", rx, valid_sz)
            .withOutputParameterReturning("rx_sz", &valid_sz, sizeof(valid_sz));

    auto rc = hm10_sync::init();

    CHECK_EQUAL(ecl::err::ok, rc);

    mock().checkExpectations();
}

TEST(hm10_sync_mode, init_invalid_resp)
{
    const char invalid_resp[] = "BZ";
    size_t inv_sz = sizeof(invalid_resp) - 1;

    mock("serial_mock").expectOneCall("init");
    mock("serial_mock").expectOneCall("nonblock");

    // HM10 will try to query module 3 times before failing dur to invalid response.

    mock("serial_mock").expectNCalls(3, "send_buf")
            .withStringParameter("tx", "AT");
    mock("serial_mock").expectNCalls(3, "recv_buf")
            .withOutputParameterReturning("rx", invalid_resp, inv_sz)
            .withOutputParameterReturning("rx_sz", &inv_sz, sizeof(inv_sz));

    auto rc = hm10_sync::init();

    CHECK_EQUAL(ecl::err::inval, rc);

    mock().checkExpectations();
}

TEST(hm10_sync_mode, init_no_resp)
{
    // Case when no responce from module.
    mock("serial_mock").disable();
    hm10_serial_mock::generate_wouldblock = true;

    // Query module

    auto rc = hm10_sync::init();

    CHECK_EQUAL(ecl::err::timedout, rc);

    mock().checkExpectations();
}

//------------------------------------------------------------------------------

TEST_GROUP(hm10_sync_mode_cmd)
{
    void setup()
    {
        do_valid_init();
        mock().clear();
    }

    void teardown()
    {
        hm10_serial_mock::generate_wouldblock = false;
        mock().clear();
    }
};

//------------------------------------------------------------------------------

template<typename Runner>
static void hm10_check_command(Runner r, ecl::err exp_ret, const std::string &exp_tx, const std::string &rx)
{
    auto rx_len = rx.size();

    mock("serial_mock").expectOneCall("send_buf")
        .withStringParameter("tx", exp_tx.c_str());

    mock("serial_mock").expectOneCall("recv_buf")
            .withOutputParameterReturning("rx", rx.c_str(), rx_len)
            .withOutputParameterReturning("rx_sz", &rx_len, sizeof(rx_len));

    auto rc = r();
    CHECK_EQUAL(exp_ret, rc);
}

TEST(hm10_sync_mode_cmd, set_pin)
{

    auto r = []() { return  hm10_sync::set_pin(123123); };
    hm10_check_command(r, ecl::err::ok, "AT+PASS123123", "OK+Set:123123");
}

TEST(hm10_sync_mode_cmd, set_pin_inval)
{
    auto r = []() { return  hm10_sync::set_pin(123123); };
    hm10_check_command(r, ecl::err::inval, "AT+PASS123123", "OK+Set:000000");
}

TEST(hm10_sync_mode_cmd, get_pin)
{
    uint32_t pin;
    auto r = [&pin]() { return hm10_sync::get_pin(pin); };
    hm10_check_command(r, ecl::err::ok, "AT+PASS?", "OK+PASS:012345");
    CHECK_EQUAL(12345, pin);
}

TEST(hm10_sync_mode_cmd, get_pin_inv_header)
{

    uint32_t pin;
    auto r = [&pin]() { return hm10_sync::get_pin(pin); };
    hm10_check_command(r, ecl::err::inval, "AT+PASS?", "ZZZZZZZZZZZ012345");
}

TEST(hm10_sync_mode_cmd, get_pin_inv_pin)
{
    uint32_t pin;
    auto r = [&pin]() { return hm10_sync::get_pin(pin); };
    hm10_check_command(r, ecl::err::inval, "AT+PASS?", "OK+PASS:ABCDEFG");
}

TEST(hm10_sync_mode_cmd, set_imme)
{
    auto r = []() { return hm10_sync::set_immediate(); };
    hm10_check_command(r, ecl::err::ok, "AT+IMME1", "OK+Set:1");
}

TEST(hm10_sync_mode_cmd, set_imme_inval)
{
    auto r = []() { return hm10_sync::set_immediate(); };
    hm10_check_command(r, ecl::err::inval, "AT+IMME1", "FOOBARFOOBARFOO");
}

TEST(hm10_sync_mode_cmd, get_imme)
{
    bool state;
    auto r = [&state]() { return hm10_sync::get_immediate(state); };
    hm10_check_command(r, ecl::err::ok, "AT+IMME?", "OK+Get:1");
    CHECK_EQUAL(true, state);
}

TEST(hm10_sync_mode_cmd, get_imme_inval)
{
    bool state;
    auto r = [&state]() { return hm10_sync::get_immediate(state); };
    hm10_check_command(r, ecl::err::inval, "AT+IMME?", "OK+Get:Z");
}

TEST(hm10_sync_mode_cmd, disconnect)
{
    auto r = []() { return hm10_sync::disconnect(); };
    hm10_check_command(r, ecl::err::ok, "AT", "OK");
}

TEST(hm10_sync_mode_cmd, disconnect_inval)
{
    auto r = []() { return hm10_sync::disconnect(); };
    hm10_check_command(r, ecl::err::inval, "AT", "FOO");
}


#if 0

//------------------------------------------------------------------------------

TEST_GROUP(hm10_sync_mode_inited)
{
    void setup()
    {
        // Just repeat valid init procedure here.

        mock().disable();

        const char valid_resp[] = "OK";

        mock("serial_mock").expectOneCall("init");
        mock("serial_mock").expectOneCall("lock");

        // Responses are not null-terminated (TODO: double check this)
        size_t valid_sz = sizeof(valid_resp) - 1;

        // Query module
        mock("serial_mock").expectOneCall("set_buffers");
        mock("serial_mock").expectOneCall("xfer")
                .withOutputParameterReturning("rx", valid_resp, sizeof(valid_resp) - 1)
                .withOutputParameterReturning("rx_sz", &valid_sz, sizeof(valid_sz))
                .withStringParameter("tx", "AT");

        auto rc = hm10_sync::init();

        CHECK_EQUAL(ecl::err::ok, rc);

        mock().enable();
    }

    void teardown()
    {
        hm10_serial_mock::cleanup();
        mock().clear();
    }
};

TEST(hm10_sync_mode_inited, send)
{
    const char send_buf[] = "HelloBtWorld";

    // No response is required
    size_t resp_sz = 0;
    size_t send_sz = sizeof(send_buf);

    mock("serial_mock").expectOneCall("set_buffers");
    mock("serial_mock").expectOneCall("xfer")
            .withStringParameter("tx", send_buf)
            .withOutputParameterReturning("rx", "", 0)
            .withOutputParameterReturning("rx_sz", &resp_sz, sizeof(resp_sz));

    auto rc = hm10_sync::send((const uint8_t *)send_buf, send_sz);

    CHECK_EQUAL(ecl::err::ok, rc);
    CHECK_EQUAL(sizeof(send_buf), send_sz);

    mock().checkExpectations();
}

TEST(hm10_sync_mode_inited, recv)
{
    const char ref_buf[] = "1234567890";
    const size_t ref_sz = sizeof(ref_buf);

    char recv_buf[ref_sz + 20] = {0};
    size_t recv_sz = sizeof(recv_buf);

    mock("serial_mock").expectOneCall("set_buffers");
    mock("serial_mock").expectOneCall("xfer")
            .withOutputParameterReturning("rx", ref_buf, sizeof(ref_buf))
            .withOutputParameterReturning("rx_sz", &ref_sz, sizeof(ref_sz))
            .withStringParameter("tx", ""); // No TX in this case

    auto rc = hm10_sync::recv((uint8_t *)recv_buf, recv_sz);

    CHECK_EQUAL(ecl::err::ok, rc);
    CHECK_EQUAL(ref_sz, recv_sz);
    CHECK_EQUAL(std::string(ref_buf), std::string(recv_buf));

    mock().checkExpectations();
}
#endif

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
