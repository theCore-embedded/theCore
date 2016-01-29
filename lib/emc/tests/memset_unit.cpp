#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>

// Relative path when testing libc-like functions is unavoidable
#include "../export/string.h"

// For convinience
auto &our_memset = LIBC_FUNCTION(memset);

static constexpr uint8_t   canary_byte     = 0xab;
static constexpr uint8_t   test_byte       = 0xe1;
static constexpr auto      canary_size     = 0xff;
static constexpr auto      data_size       = 0xff;
static constexpr auto      arr_size        = data_size + canary_size * 2;

// Special struct used to catch buffer overflows
struct test_chunk
{
    uint8_t head_canary[canary_size];
    uint8_t data[data_size];
    uint8_t tail_canary[canary_size];
}  __attribute__((packed));

test_chunk chunk;

TEST_GROUP(memset_test)
{
    void setup()
    {
        // Fill head and tail canaries
        std::fill(chunk.head_canary, chunk.head_canary + canary_size, canary_byte);
        std::fill(chunk.tail_canary, chunk.tail_canary + canary_size, canary_byte);
    }

    void teardown()
    {
        // Do nothing
    }
};

TEST(memset_test, retval)
{

}

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
