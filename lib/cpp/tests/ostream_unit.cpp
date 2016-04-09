#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

#include <string>
#include "ecl/ostream.hpp"

class device_t
{
public:
    int test_val = 0;
    int *p_test_val = &test_val;
    std::string p_test_input_str;
    int test_input_number = 0;

    int write(uint8_t *c, int number)
    {
        test_input_number = number;
        *p_test_val = *p_test_val + 1;
        p_test_input_str.append(reinterpret_cast<char*>(c),
                                number);
        return 0;
    }

    int write(const uint8_t *c, int number)
    {
        *p_test_val = *p_test_val + 1;
        p_test_input_str.append(reinterpret_cast<const char*>(c),
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
    CHECK_EQUAL("A", test_device.p_test_input_str);
    CHECK_EQUAL(1, test_device.test_input_number);
}

TEST(ostream, read_char)
{
    test_ostream->operator<<('B');
    CHECK_EQUAL(1, test_device.test_val);
    CHECK_EQUAL("B", test_device.p_test_input_str);
    CHECK_EQUAL(1, test_device.test_input_number);
}

TEST(ostream, read_string)
{
    test_ostream->operator<<("Hello");
    CHECK_EQUAL(5, test_device.test_val);
    CHECK_EQUAL("Hello", test_device.p_test_input_str)
}

TEST(ostream, read_int)
{
    test_ostream->operator<<(12345);
    CHECK_EQUAL(5, test_device.test_val);
    CHECK_EQUAL("12345", test_device.p_test_input_str);

}

int main(int argc, char *argv[])
{
    //    ecl::ostream< foo > os{&foo_obj};
    //    os.put('k');

    return CommandLineTestRunner::RunAllTests(argc, argv);
}

