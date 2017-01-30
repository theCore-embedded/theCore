//! \file
//! \brief Mock time header for no-os semaphore implementation.

#ifndef MOCK_TIME_HPP_
#define MOCK_TIME_HPP_

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

#include <chrono>

extern volatile uint32_t mock_time;

static inline std::chrono::milliseconds get_ms_time()
{
    return std::chrono::milliseconds(mock_time);
}

#endif // MOCK_TIME_HPP_
