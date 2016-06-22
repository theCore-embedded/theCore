#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>
#include <limits.h>
#include <sstream>

#include "ecl/ostream.hpp"
#include "mock_device.hpp"

using ostream_unit = ecl::ostream< mock_device >;

TEST_GROUP(ostream)
{
    mock_device test_device;
    ostream_unit *test_ostream;

    void setup()
    {
        test_ostream = new ostream_unit(&test_device);
        test_device.idx = 0;
        test_device.test_input_str = "";
        test_device.error_mode = false;
    }

    void teardown()
    {
        mock().clear();
        delete test_ostream;
    }
};

TEST(ostream, put)
{
    test_ostream->put('A');
    CHECK_EQUAL("A", test_device.test_input_str);
}

TEST(ostream, write_char)
{
    test_ostream->operator<<('B');
    CHECK_EQUAL("B", test_device.test_input_str);
}

TEST(ostream, write_char_error_case)
{
    test_device.error_mode = true;

    mock().expectOneCall("write").andReturnValue(-1);
    test_ostream->operator<<('A');
    mock().checkExpectations();
    CHECK_EQUAL("", test_device.test_input_str);
}

TEST(ostream, write_string)
{
    test_ostream->operator<<("Hello");
    CHECK_EQUAL("Hello", test_device.test_input_str);
}

TEST(ostream, write_string_with_linefeed)
{
    test_ostream->operator<<("Hola, amigos!\n");
    CHECK_EQUAL("Hola, amigos!\r\n", test_device.test_input_str);
}

TEST(ostream, write_empty_string)
{
    test_ostream->operator<<("");
    CHECK_EQUAL("", test_device.test_input_str);
}

TEST(ostream, write_string_error_case)
{
    test_device.error_mode = true;

    mock().expectOneCall("write").andReturnValue(-1);
    test_ostream->operator<<("ERROR");
    mock().checkExpectations();
    CHECK_EQUAL("", test_device.test_input_str);
}

TEST(ostream, write_int)
{
    test_ostream->operator<<(12345);
    CHECK_EQUAL("12345", test_device.test_input_str);
}

TEST(ostream, write_int_error_case)
{
    test_device.error_mode = true;

    mock().expectOneCall("write").andReturnValue(-1);
    test_ostream->operator<<(987);
    mock().checkExpectations();
    CHECK_EQUAL("", test_device.test_input_str);
}

TEST(ostream, write_int_min)
{
    std::basic_stringstream<char> ss;
    int input_num = INT_MIN;
    char out[255];

    ss << input_num;
    ss.get(out, sizeof(out));
    test_ostream->operator<<(input_num);
    CHECK_EQUAL(out, test_device.test_input_str);
}

TEST(ostream, write_int_max)
{
    std::basic_stringstream<char> ss;
    int input_num = INT_MAX;
    char out[255];

    ss << input_num;
    ss.get(out, sizeof(out));
    test_ostream->operator<<(input_num);
    CHECK_EQUAL(out, test_device.test_input_str);
}

TEST(ostream, write_zero)
{
    test_ostream->operator<<(0);
    CHECK_EQUAL("0", test_device.test_input_str);
}

TEST(ostream, write_unsigned_int)
{
    unsigned int input_num = 11111;
    test_ostream->operator<<(input_num);
    CHECK_EQUAL("11111", test_device.test_input_str);
}

TEST(ostream, write_unsigned_int_max)
{
    std::basic_stringstream<char> ss;
    unsigned int input_num = UINT_MAX;
    char out[255];

    ss << input_num;
    ss.get(out, sizeof(out));
    test_ostream->operator<<(input_num);
    CHECK_EQUAL(out, test_device.test_input_str);
}

TEST(ostream, write_unsigned_int_zero)
{
    unsigned int input_num = 0;
    test_ostream->operator<<(input_num);
    CHECK_EQUAL("0", test_device.test_input_str);
}

TEST(ostream, write_unsigned_int_error_case)
{
    test_device.error_mode = true;
    unsigned int input_num = 11111;

    mock().expectOneCall("write").andReturnValue(-1);
    test_ostream->operator<<(input_num);
    mock().checkExpectations();
    CHECK_EQUAL("", test_device.test_input_str);
}


int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
