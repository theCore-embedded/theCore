#ifndef HM10_SERIAL_MOCK
#define HM10_SERIAL_MOCK

#include <CppUTestExt/MockSupport.h>

namespace ecl
{

//------------------------------------------------------------------------------
// HM10 block mode mocks

template<typename Ignored>
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

template<typename Ignored>
bool hm10_serial_mock<Ignored>::generate_wouldblock;

template<typename Ignored>
using serial = hm10_serial_mock<Ignored>;

} // namespace ecl

#endif // HM10_SERIAL_MOCK
