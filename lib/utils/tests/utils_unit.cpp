/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ecl/utils.hpp"

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

// Mocks

struct mock_type1
{
    void fn()  { mock("mock_type1").actualCall("fn"); }
};

struct mock_type2
{
    void fn()  { mock("mock_type2").actualCall("fn"); }
};

struct mock_type3
{
    void fn()  { mock("mock_type3").actualCall("fn"); }
};

struct mock_type4
{
    void fn()  { mock("mock_type4").actualCall("fn"); }
};

TEST_GROUP(tuple_iteration)
{
    void setup()
    {
    }

    void teardown()
    {
        mock().clear();
    }
};

TEST(tuple_iteration, basic)
{
    mock("mock_type1").expectOneCall("fn");
    mock("mock_type2").expectOneCall("fn");
    mock("mock_type3").expectOneCall("fn");
    mock("mock_type4").expectOneCall("fn");

    std::tuple<mock_type1, mock_type2, mock_type3, mock_type4> test_tuple;

    ecl::for_each(test_tuple, [](auto &t){ t.fn(); });

    mock().checkExpectations();
}

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}


