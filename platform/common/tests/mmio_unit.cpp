/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Memory mapped I/O abstractions unit test.

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

#include "common/mmio.hpp"

using namespace ecl::mmio;

//------------------------------------------------------------------------------

TEST_GROUP(gen_mask)
{

};

TEST(gen_mask, unshifted)
{
    // 0 is prohibited value.
    // CHECK_EQUAL(0, generate_unshifted_mask(0));
    CHECK_EQUAL(1, generate_unshifted_mask(1));
    CHECK_EQUAL(0b111111, generate_unshifted_mask(6));
    CHECK_EQUAL(0xffffffff, generate_unshifted_mask(32));
}

TEST(gen_mask, shifted)
{
    CHECK_EQUAL(1, generate_mask(1, 0));
    CHECK_EQUAL(0b1111111111, generate_mask(10, 0));
    CHECK_EQUAL(0b111000000000, generate_mask(3, 9));
    CHECK_EQUAL(0x80000000, generate_mask(1, 31));
    CHECK_EQUAL(0xffffffff, generate_mask(32, 0));
}

//------------------------------------------------------------------------------

TEST_GROUP(mutability_traits)
{

};

TEST(mutability_traits, ro)
{
    const volatile uint32_t mock_hw_reg = 0b11001100111;

    // Extract first bit
    CHECK_EQUAL(1, ro_mut::read(&mock_hw_reg, 0, 0b1));

    // Extract group at the start
    CHECK_EQUAL(0b0111, ro_mut::read(&mock_hw_reg, 0, 0x7));

    // Extract something in the middle
    CHECK_EQUAL(0b10011, ro_mut::read(&mock_hw_reg, 5, 0b1111100000));
}

TEST(mutability_traits, wo)
{
    volatile uint32_t mock_hw_reg;

    // Write something at the start
    wo_mut::write(&mock_hw_reg, 0, 0x3f, 0b110011);
    CHECK_EQUAL(0b110011, mock_hw_reg);

    // Write something in the middle
    wo_mut::write(&mock_hw_reg, 5, 0b1111100000, 0b10101);
    CHECK_EQUAL(0b1010100000, mock_hw_reg);
}

TEST(mutability_traits, rw_mut)
{
    // Initial value matters when dealing with rw registers
    volatile uint32_t mock_hw_reg = 0;

    // Tracks all modifications
    uint32_t expected;

    // Check initial state
    CHECK_EQUAL(0, rw_mut::read(&mock_hw_reg, 0, 0xffffffff));

    // Write something at the start
    expected = 0xaa;
    rw_mut::write(&mock_hw_reg, 0, 0xff, 0xaa);
    CHECK_EQUAL(expected, mock_hw_reg);
    CHECK_EQUAL(0xaa, rw_mut::read(&mock_hw_reg, 0, 0xff));

    // Test for erasing old data from a register in case of overlap.
    expected = 0xbba;
    rw_mut::write(&mock_hw_reg, 4, 0xff0, 0xbb);
    CHECK_EQUAL(expected, mock_hw_reg);
    CHECK_EQUAL(0xbb, rw_mut::read(&mock_hw_reg, 4, 0xff0));

    // No overlap test
    expected = 0xcc000bba;
    rw_mut::write(&mock_hw_reg, 24, 0xff000000, 0xcc);
    CHECK_EQUAL(expected, mock_hw_reg);
    CHECK_EQUAL(0xcc, rw_mut::read(&mock_hw_reg, 24, 0xff000000));
}

//------------------------------------------------------------------------------

struct mock_mut
{
    static uint32_t read(volatile const uint32_t *device, uint32_t offset, uint32_t mask)
    {
        last_passed_device = device;

        return mock().actualCall("read")
                  .withParameter("offset", offset)
                  .withParameter("mask", mask)
                  .returnUnsignedIntValue();
    }

    static void write(volatile uint32_t *device, uint32_t offset, uint32_t mask, uint32_t value)
    {
        mock().actualCall("write")
              .withParameter("offset", offset)
              .withParameter("mask", mask)
              .withParameter("value", value);

        last_passed_device = device;
    }

    // withParameter() of the CppUTest does not support volatile modifier.
    static volatile const uint32_t *last_passed_device;
};

volatile const uint32_t *mock_mut::last_passed_device;

TEST_GROUP(mmio_reg)
{
    static constexpr std::uintptr_t device_addr = 0xcaffe;
    volatile uint32_t *expected_device = reinterpret_cast<volatile uint32_t*>(device_addr);

    uint32_t expected_ret = 0xabcd;
    uint32_t write_value  = 0xffbb;

    void setup()
    {
        mock_mut::last_passed_device = nullptr;
    }

    void teardown()
    {
        mock().clear();
    }
};

TEST(mmio_reg, at_beginning)
{
    constexpr auto offset = 0;
    constexpr auto width = 32;

    using reg_under_test = reg<mock_mut, device_addr, offset, width>;

    mock().expectOneCall("read")
            .withParameter("offset", offset)
            .withParameter("mask", 0xffffffff)
            .andReturnValue(expected_ret);

    auto ret = reg_under_test::read();
    CHECK_EQUAL(expected_ret, ret);
    CHECK_EQUAL(expected_device, mock_mut::last_passed_device);

    mock().expectOneCall("write")
            .withParameter("offset", offset)
            .withParameter("mask", 0xffffffff)
            .withParameter("value", write_value);

    reg_under_test::write(write_value);

    mock().checkExpectations();
}

TEST(mmio_reg, at_middle)
{
    constexpr auto offset = 9;
    constexpr auto width = 11;

    using reg_under_test = reg<mock_mut, device_addr, offset, width>;

    mock().expectOneCall("read")
            .withParameter("offset", offset)
            .withParameter("mask", 0xffe00)
            .andReturnValue(expected_ret);

    auto ret = reg_under_test::read();
    CHECK_EQUAL(expected_ret, ret);
    CHECK_EQUAL(expected_device, mock_mut::last_passed_device);

    mock().expectOneCall("write")
    .withParameter("offset", offset)
    .withParameter("mask", 0xffe00)
    .withParameter("value", write_value);

    reg_under_test::write(write_value);

    mock().checkExpectations();
}

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
