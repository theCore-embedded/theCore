/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>
#include <sstream>

#include "ecl/istream.hpp"
#include "mock_device.hpp"

using istream_unit = ecl::istream< mock_device >;

TEST_GROUP(istream)
{
    mock_device test_device;
    istream_unit *test_istream;

    void setup()
    {
        test_istream = new istream_unit(&test_device);
        test_device.idx = 0;
    }

    void teardown()
    {
        mock().clear();
        delete test_istream;
    }
};

TEST(istream, get)
{
    char input_char;
    test_device.test_input_str = "A";
    test_istream->get(input_char);
    CHECK_EQUAL('A', input_char);
}

TEST(istream, read_char)
{
    char output_char;

    test_device.test_input_str = "B";
    test_istream->operator>>(output_char);
    CHECK_EQUAL('B', output_char);

    test_device.idx = 0;
    test_device.test_input_str = "A B\nC\tD    E\n\n\n\nF";

    test_istream->operator>>(output_char);
    CHECK_EQUAL('A', output_char);

    test_istream->operator>>(output_char);
    CHECK_EQUAL('B', output_char);

    test_istream->operator>>(output_char);
    CHECK_EQUAL('C', output_char);

    test_istream->operator>>(output_char);
    CHECK_EQUAL('D', output_char);

    test_istream->operator>>(output_char);
    CHECK_EQUAL('E', output_char);

    test_istream->operator>>(output_char);
    CHECK_EQUAL('F', output_char);
}

TEST(istream, read_char_error_case)
{
    char output_char = 'A';
    test_device.test_input_str = "Z";
    test_device.error_mode = true;

    mock().expectOneCall("read").andReturnValue(-1);
    test_istream->operator>>(output_char);
    mock().checkExpectations();
    CHECK_EQUAL('A', output_char);

    test_device.error_mode = false;
}

TEST(istream, read_string)
{
    const int size = 100;
    char output_string[size] = "\0";
    test_device.test_input_str = "Hello\n";
    test_istream->operator>>(output_string);
    STRCMP_EQUAL("Hello", output_string);
}

TEST(istream, read_string_with_special_chars)
{
    const int size = 100;
    char output_string[size] = "\0";

    test_device.test_input_str = " \nHE  L\n";
    test_istream->operator>>(output_string);
    STRCMP_EQUAL("HE", output_string);
    test_istream->operator>>(output_string);
    STRCMP_EQUAL("L", output_string);

    test_device.idx = 0;
    test_device.test_input_str = "\t   !\\HEL\t\n";
    test_istream->operator>>(output_string);
    STRCMP_EQUAL("!\\HEL", output_string);
}

TEST(istream, read_string_linefeed)
{
    const int size = 100;
    char output_string[size] = "\0";

    test_device.test_input_str = "\n";
    test_istream->operator>>(output_string);
    STRCMP_EQUAL("", output_string);
}

TEST(istream, read_string_error_case)
{
    const int size = 100;
    char output_string[size] = "SUCCESS";
    test_device.test_input_str = "ERROR";
    test_device.error_mode = true;

    mock().expectOneCall("read").andReturnValue(-1);
    test_istream->operator>>(output_string);
    mock().checkExpectations();
    STRCMP_EQUAL("\0", output_string);

    test_device.error_mode = false;
}

TEST(istream, read_int)
{
    int output_int;
    int ss_out;
    std::basic_stringstream<char> ss;
    test_device.test_input_str = "  123456";

    ss << test_device.test_input_str;
    ss >> ss_out;
    test_istream->operator>>(output_int);
    CHECK_EQUAL(ss_out, output_int);
}

TEST(istream, read_int_min)
{
    int output_int;
    int ss_out;
    std::basic_stringstream<char> ss;
    test_device.test_input_str = "  -2147483647 ";

    ss << test_device.test_input_str;
    ss >> ss_out;
    test_istream->operator>>(output_int);
    CHECK_EQUAL(ss_out, output_int);
}

TEST(istream, read_int_with_linefeeds)
{
    int output_int;
    test_device.test_input_str = "\n12\n34";
    test_istream->operator>>(output_int);
    CHECK_EQUAL(12, output_int);
    test_istream->operator>>(output_int);
    CHECK_EQUAL(34, output_int);
}

//TODO: operator>>(int& value) should pass '-' to the next number and make it negative
//Expected: "987-654-321" should return the following numbers: 987, -654, -321
//Actual: testcase is successful only with spaces before '-'
TEST(istream, read_int_with_hyphens)
{
    int output_int;
    int ss_out;
    std::basic_stringstream<char> ss;
    test_device.test_input_str = "987 -654 -321";
    ss << test_device.test_input_str;

    for (int i = 0; i < 3; i++) {
        ss >> ss_out;
        test_istream->operator>>(output_int);
        CHECK_EQUAL(ss_out, output_int);
    }
}
/*
TEST(istream, read_int_with_hyphens1)
{
    int output_int;
    int ss_out;
    std::basic_stringstream<char> ss;
    test_device.test_input_str = "987-654-321";
    ss << test_device.test_input_str;

    for (int i = 0; i < 3; i++) {
        ss >> ss_out;
        test_istream->operator>>(output_int);
        CHECK_EQUAL(ss_out, output_int);
    }
}
*/

TEST(istream, read_int_zero)
{
    int output_int;
    test_device.test_input_str = "0";
    test_istream->operator>>(output_int);
    CHECK_EQUAL(0, output_int);
}

//TODO: INT_MAX can be more than 2 ^ 32.
//This case becomes useless in such systems
TEST(istream, read_int_bigger_than_max)
{
    int output_int;
    int ss_out;
    std::basic_stringstream<char> ss;
    test_device.test_input_str = "214748364700";

    ss << test_device.test_input_str;
    ss >> ss_out;
    test_istream->operator>>(output_int);
    CHECK_EQUAL(ss_out, output_int);
}

TEST(istream, read_int_error_case)
{
    int output_int = 123;
    test_device.test_input_str = "000";
    test_device.error_mode = true;

    mock().expectOneCall("read").andReturnValue(-1);
    test_istream->operator>>(output_int);
    mock().checkExpectations();
    CHECK_EQUAL(123, output_int);

    test_device.error_mode = false;
}

TEST(istream, read_unsigned_int)
{
    unsigned int output_int;
    test_device.test_input_str = " 234 ";
    test_istream->operator>>(output_int);
    CHECK_EQUAL(234, output_int);
}

TEST(istream, read_unsigned_zero)
{
    unsigned int output_int;
    test_device.test_input_str = "\n0\n";
    test_istream->operator>>(output_int);
    LONGS_EQUAL(0, output_int);
}

TEST(istream, read_unsigned_consequent)
{
    unsigned int output_int;
    test_device.test_input_str = "987\n654";
    test_istream->operator>>(output_int);
    LONGS_EQUAL(987, output_int);

    test_istream->operator>>(output_int);
    LONGS_EQUAL(654, output_int);
}

TEST(istream, read_unsigned_int_error_case)
{
    unsigned int output_int = 123;
    test_device.test_input_str = "000";
    test_device.error_mode = true;

    mock().expectOneCall("read").andReturnValue(-1);
    test_istream->operator>>(output_int);
    mock().checkExpectations();
    CHECK_EQUAL(123, output_int);

    test_device.error_mode = false;
}

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}

