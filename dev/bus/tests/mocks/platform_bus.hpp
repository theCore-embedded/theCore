#ifndef MOCK_PLATFORM_BUS_HPP_
#define MOCK_PLATFORM_BUS_HPP_

// Mocking platform-side bus

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <ecl/err.hpp>
#include <iostream>
class platform_mock
{
public:
    enum class event
    {
        tx_half_complete,
        rx_half_complete,
        tx_complete,
        rx_complete,
        tx_err,
        rx_err,
    };

    ecl::err init()
    {
        mock("platform_bus").actualCall("init");
        return static_cast< ecl::err >
                (mock("platform_bus").returnIntValueOrDefault(0));
    }

    void reset_buffers()
    {
        mock("platform_bus").actualCall("reset_buffers");
    }

    void set_tx(const uint8_t *tx, size_t size)
    {
        mock("platform_bus")
                .actualCall("set_tx")
                .withParameter("tx_buf", tx)
                .withParameter("size", size);
    }

    void set_rx(uint8_t *rx, size_t size)
    {
        mock("platform_bus")
                .actualCall("set_rx")
                .withParameter("rx_buf", rx)
                .withParameter("size", size);
    }
};

#endif
