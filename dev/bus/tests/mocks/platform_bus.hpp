/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MOCK_PLATFORM_BUS_HPP_
#define MOCK_PLATFORM_BUS_HPP_

// Mocking platform-side bus

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <ecl/err.hpp>
#include <iostream>

#include <dev/bus.hpp>

class platform_mock
{
public:
//------------------------------------------------------------------------------
// Bus interface itself

    using channel    = ecl::bus_channel;
    using event      = ecl::bus_event;
    using handler_fn = ecl::bus_handler;

    static ecl::err init()
    {
        mock("platform_bus").actualCall("init");
        return static_cast< ecl::err >
                (mock("platform_bus").returnIntValueOrDefault(0));
    }

    static void reset_buffers()
    {
        mock("platform_bus").actualCall("reset_buffers");
    }

    static void set_tx(const uint8_t *tx, size_t size)
    {
        mock("platform_bus")
                .actualCall("set_tx")
                .withParameter("tx_buf", tx)
                .withParameter("size", size);
    }

    static void set_rx(uint8_t *rx, size_t size)
    {
        mock("platform_bus")
                .actualCall("set_rx")
                .withParameter("rx_buf", rx)
                .withParameter("size", size);
    }

    static void set_tx(size_t size, uint8_t fill_byte)
    {
        mock("platform_bus")
                .actualCall("set_tx")
                .withParameter("tx_size", size)
                .withParameter("fill_byte", fill_byte);
    }

    static void set_handler(const handler_fn &handler)
    {
        mock("platform_bus").actualCall("set_handler");
        m_handler = handler;
    }

    static void reset_handler()
    {
        mock("platform_bus").actualCall("reset_handler");
        m_handler = handler_fn{};
    }

    static ecl::err do_xfer()
    {
        mock("platform_bus").actualCall("do_xfer");
        return static_cast< ecl::err >
                (mock("platform_bus").returnIntValueOrDefault(0));
    }

    static ecl::err cancel_xfer()
    {
        mock("platform_bus").actualCall("cancel_xfer");
        return static_cast<ecl::err>
                (mock("platform_bus").returnIntValueOrDefault(0));
    }

//------------------------------------------------------------------------------
// Internally used by the test

    static void invoke(channel ch, event e, size_t total)
    {
        m_handler(ch, e, total);
    }

private:
    static handler_fn m_handler;
};

// TODO: move it to cpp file to avoid link errors in future
platform_mock::handler_fn platform_mock::m_handler = platform_mock::handler_fn{};

#endif
