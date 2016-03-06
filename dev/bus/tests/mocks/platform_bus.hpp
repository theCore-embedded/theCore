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
//------------------------------------------------------------------------------
// Bus interface itself
    enum class event
    {
        tx_half_complete,
        rx_half_complete,
        tx_complete,
        rx_complete,
        tx_err,
        rx_err,
        xfer_done, // End of transfer
    };

    using handler_fn = std::function< void(event type) >;

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

    void set_tx(size_t size, uint8_t fill_byte)
    {
        mock("platform_bus")
                .actualCall("set_tx")
                .withParameter("rx_size", size)
                .withParameter("fill_byte", fill_byte);
    }

    void set_handler(const handler_fn &handler)
    {
        mock("platform_bus").actualCall("set_handler");
        m_handler = handler;
    }

    void reset_handler()
    {
        mock("platform_bus").actualCall("reset_handler");
        m_handler = handler_fn{};
    }

    ecl::err do_xfer()
    {
        mock("platform_bus").actualCall("do_xfer");
        return static_cast< ecl::err >
                (mock("platform_bus").returnIntValueOrDefault(0));
    }

//------------------------------------------------------------------------------
// Internally used by the test

    static void invoke(event e)
    {
        m_handler(e);
    }

private:
    static handler_fn m_handler;
};

// TODO: move it to cpp file to avoid link errors in future
platform_mock::handler_fn platform_mock::m_handler = platform_mock::handler_fn{};

#endif
