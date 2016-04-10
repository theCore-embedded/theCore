#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

#include <string>
#include "ecl/ostream.hpp"

#include <iostream>

class device_t
{
public:
    int test_val = 0;
    int *p_test_val = &test_val;
    std::string test_input_str;

    int write(uint8_t *c, int number)
    {
        *p_test_val = *p_test_val + 1;
        test_input_str.append(reinterpret_cast<char*>(c),
                                number);
        return 0;
    }

    int write(const uint8_t *c, int number)
    {
        *p_test_val = *p_test_val + 1;
        test_input_str.append(reinterpret_cast<const char*>(c),
                                number);
        return 0;
    }

};

using ostream_t = ecl::ostream< device_t >;

TEST_GROUP(ostream)
{
    device_t test_device;
    ostream_t *test_ostream;

    void setup()
    {
        test_ostream = new ostream_t(&test_device);
    }

    void teardown()
    {
        delete test_ostream;
    }
};

TEST(ostream, put)
{
    test_ostream->put('A');
    CHECK_EQUAL(1, test_device.test_val);
    CHECK_EQUAL("A", test_device.test_input_str);
}

TEST(ostream, read_char)
{
    test_ostream->operator<<('B');
    CHECK_EQUAL(1, test_device.test_val);
    CHECK_EQUAL("B", test_device.test_input_str);
}

TEST(ostream, read_string)
{
    test_ostream->operator<<("Hello");
    CHECK_EQUAL(5, test_device.test_val);
    CHECK_EQUAL("Hello", test_device.test_input_str);

    test_device.test_val = 0;
    test_device.test_input_str = "";
    test_ostream->operator<<("Hola, amigos!\n");
    CHECK_EQUAL(15, test_device.test_val);
    CHECK_EQUAL("Hola, amigos!\r\n", test_device.test_input_str);
}

TEST(ostream, read_int)
{
    test_ostream->operator<<(12345);
    CHECK_EQUAL(5, test_device.test_val);
    CHECK_EQUAL("12345", test_device.test_input_str);

    test_device.test_val = 0;
    test_device.test_input_str = "";
    int input_num = -2147483647;
    std::cout << input_num;
    test_ostream->operator<<(input_num);
    CHECK_EQUAL(11, test_device.test_val);
    CHECK_EQUAL("-2147483647", test_device.test_input_str);

    test_device.test_val = 0;
    test_device.test_input_str = "";
    test_ostream->operator<<(0);
    CHECK_EQUAL(1, test_device.test_val);
    CHECK_EQUAL("0", test_device.test_input_str);
/*
    unsigned int input_num = 4294967295;
    test_device.test_val = 0;
    test_device.test_input_str = "";
    test_ostream->operator<<(input_num);
    CHECK_EQUAL(10, test_device.test_val);
    CHECK_EQUAL("4294967295", test_device.test_input_str);
    */
}

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}

