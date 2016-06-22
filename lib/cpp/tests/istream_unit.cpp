#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

#include <string>
#include "ecl/ostream.hpp"
#include "ecl/istream.hpp"

#include <iostream>

class device_t
{
public:
    int test_val = 0;
    int *p_test_val = &test_val;
    std::string test_input_str;

    size_t write(uint8_t *c, int size)
    {
        *p_test_val += 1;
        test_input_str.append(reinterpret_cast<char*>(c),
                              size);
        return 0;
    }

    size_t write(const uint8_t *c, int size)
    {
        *p_test_val += 1;
        test_input_str.append(reinterpret_cast<const char*>(c),
                              size);
        return 0;
    }

    size_t read(uint8_t *c, int size)
    {
        (void)size;
        *c = test_input_str[test_val];
        *p_test_val += 1;

        return 1;
    }
};

using ostream_t = ecl::ostream< device_t >;
using istream_t = ecl::istream< device_t >;

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

TEST(ostream, write_char)
{
    test_ostream->operator<<('B');
    CHECK_EQUAL(1, test_device.test_val);
    CHECK_EQUAL("B", test_device.test_input_str);
}

TEST(ostream, write_string)
{
    test_ostream->operator<<("Hello");
    CHECK_EQUAL(5, test_device.test_val);
    CHECK_EQUAL("Hello", test_device.test_input_str);

    test_device.test_val = 0;
    test_device.test_input_str = "";
    test_ostream->operator<<("Hola, amigos!\n");
    CHECK_EQUAL(15, test_device.test_val);
    CHECK_EQUAL("Hola, amigos!\r\n", test_device.test_input_str);

    test_device.test_val = 0;
    test_device.test_input_str = "";
    test_ostream->operator<<("");
    CHECK_EQUAL(0, test_device.test_val);
    CHECK_EQUAL("", test_device.test_input_str);

    char input_string[] = "123";
    test_device.test_val = 0;
    test_device.test_input_str = "";
    test_ostream->operator<<(input_string);
    CHECK_EQUAL(3, test_device.test_val);
    CHECK_EQUAL("123", test_device.test_input_str);
}

TEST(ostream, write_int)
{
    test_ostream->operator<<(12345);
    CHECK_EQUAL(5, test_device.test_val);
    CHECK_EQUAL("12345", test_device.test_input_str);

    test_device.test_val = 0;
    test_device.test_input_str = "";
    int input_num = -2147483647;
    test_ostream->operator<<(input_num);
    CHECK_EQUAL(11, test_device.test_val);
    CHECK_EQUAL("-2147483647", test_device.test_input_str);

    test_device.test_val = 0;
    test_device.test_input_str = "";
    test_ostream->operator<<(0);
    CHECK_EQUAL(1, test_device.test_val);
    CHECK_EQUAL("0", test_device.test_input_str);
}

TEST(ostream, write_unsigned_int)
{
    unsigned int input_num = 4294967295;
    test_device.test_val = 0;
    test_device.test_input_str = "";
    test_ostream->operator<<(input_num);
    CHECK_EQUAL(10, test_device.test_val);
    CHECK_EQUAL("4294967295", test_device.test_input_str);

    unsigned int input_num1 = 0;
    test_device.test_val = 0;
    test_device.test_input_str = "";
    test_ostream->operator<<(input_num1);
    CHECK_EQUAL(1, test_device.test_val);
    CHECK_EQUAL("0", test_device.test_input_str);

    unsigned int input_num2 = 11111;
    test_device.test_val = 0;
    test_device.test_input_str = "";
    test_ostream->operator<<(input_num2);
    CHECK_EQUAL(5, test_device.test_val);
    CHECK_EQUAL("11111", test_device.test_input_str);

}

TEST_GROUP(istream)
{
    device_t test_device;
    istream_t *test_istream;

    void setup()
    {
        test_istream = new istream_t(&test_device);
    }

    void teardown()
    {
        delete test_istream;
    }
};

TEST(istream, put)
{
    char input_char;
    test_device.test_input_str[0] = 'A';
    test_istream->get(input_char);
    CHECK_EQUAL(1, test_device.test_val);
    CHECK_EQUAL('A', input_char);
}

TEST(istream, read_char)
{
    char output_char;
    test_device.test_input_str[0] = 'B';
    test_istream->operator>>(output_char);
    CHECK_EQUAL(1, test_device.test_val);
    CHECK_EQUAL('B', output_char);

    test_device.test_val = 0;
    test_device.test_input_str = "A B\nC\tD    E\n\n\n\nF";
    test_istream->operator>>(output_char);
    CHECK_EQUAL(1, test_device.test_val);
    CHECK_EQUAL('A', output_char);

    test_istream->operator>>(output_char);
    CHECK_EQUAL(3, test_device.test_val);
    CHECK_EQUAL('B', output_char);

    test_istream->operator>>(output_char);
    CHECK_EQUAL(5, test_device.test_val);
    CHECK_EQUAL('C', output_char);

    test_istream->operator>>(output_char);
    CHECK_EQUAL(7, test_device.test_val);
    CHECK_EQUAL('D', output_char);

    test_istream->operator>>(output_char);
    CHECK_EQUAL(12, test_device.test_val);
    CHECK_EQUAL('E', output_char);

    test_istream->operator>>(output_char);
    CHECK_EQUAL(17, test_device.test_val);
    CHECK_EQUAL('F', output_char);
}

TEST(istream, read_string)
{
    const int size = 100;
    char output_string[size] = "\0";
    test_device.test_input_str = "Hello\n";
    test_istream->operator>>(output_string);
    CHECK_EQUAL(6, test_device.test_val);
    STRCMP_EQUAL("Hello", output_string);

    test_device.test_val = 0;
    test_device.test_input_str = " \nHE  L\n";
    test_istream->operator>>(output_string);
    CHECK_EQUAL(5, test_device.test_val);
    STRCMP_EQUAL("HE", output_string);

    test_device.test_val = 0;
    test_device.test_input_str = "\t   !\\HEL\t\n";
    test_istream->operator>>(output_string);
    CHECK_EQUAL(10, test_device.test_val);
    STRCMP_EQUAL("!\\HEL", output_string);

    test_device.test_val = 0;
    test_device.test_input_str = "\n";
    test_istream->operator>>(output_string);
    //CHECK_EQUAL(1, test_device.test_val);
    STRCMP_EQUAL("", output_string);
}

TEST(istream, read_int)
{
    int output_int;
    test_device.test_input_str = "  123456";
    test_istream->operator>>(output_int);
    CHECK_EQUAL(9, test_device.test_val);
    CHECK_EQUAL(123456, output_int);

    test_device.test_val = 0;
    test_device.test_input_str = "  -2147483647 ";
    test_istream->operator>>(output_int);
    CHECK_EQUAL(14, test_device.test_val);
    CHECK_EQUAL(-2147483647, output_int);

    test_device.test_val = 0;
    test_device.test_input_str = "\n12\n34";
    test_istream->operator>>(output_int);
    CHECK_EQUAL(4, test_device.test_val);
    CHECK_EQUAL(12, output_int);

    test_device.test_val = 0;
    test_device.test_input_str = "987-654-321";
    test_istream->operator>>(output_int);
    CHECK_EQUAL(4, test_device.test_val);
    CHECK_EQUAL(987, output_int);

    test_device.test_val = 0;
    test_device.test_input_str = "0";
    test_istream->operator>>(output_int);
    CHECK_EQUAL(2, test_device.test_val);
    CHECK_EQUAL(0, output_int);
/*
    test_device.test_val = 0;
    test_device.test_input_str = "214748364700";
    test_istream->operator>>(output_int);
    CHECK_EQUAL(13, test_device.test_val);
    CHECK_EQUAL(2147483647, output_int);
*/
}

TEST(istream, read_unsigned_int)
{
    unsigned int output_int;
    test_device.test_input_str = " 4294967295 ";
    test_istream->operator>>(output_int);
    CHECK_EQUAL(12, test_device.test_val);
    LONGS_EQUAL(4294967295, output_int);

    test_device.test_val = 0;
    test_device.test_input_str = "\n0\n";
    test_istream->operator>>(output_int);
    CHECK_EQUAL(3, test_device.test_val);
    LONGS_EQUAL(0, output_int);

    test_device.test_val = 0;
    test_device.test_input_str = "987\n654";
    test_istream->operator>>(output_int);
    CHECK_EQUAL(4, test_device.test_val);
    LONGS_EQUAL(987, output_int);
}

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}

