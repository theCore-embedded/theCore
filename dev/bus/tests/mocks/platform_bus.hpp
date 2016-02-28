#ifndef MOCK_PLATFORM_BUS_HPP_
#define MOCK_PLATFORM_BUS_HPP_

// Mocking platform-side bus

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <ecl/err.hpp>

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
                (mock("platform_bus").intReturnValue());
    }

    void reset()
    {
        mock("platform_bus").actualCall("reset");
    }
};

#endif
