// Relative path when testing libc-like functions is unavoidable
#include "../export/string.h"

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <algorithm>

// For convinience
static auto &our_memset = LIBC_FUNCTION(memset);

static constexpr uint8_t   canary_byte      = 0xab;
static constexpr uint8_t   test_byte        = 0xe1;
static constexpr uint8_t   initial_byte     = 0x5c;
static constexpr auto      canary_size      = 0xff;
static constexpr auto      data_size        = 0xff;
static constexpr auto      arr_size         = data_size + canary_size * 2;

// Special struct used to catch buffer overflows
struct test_chunk
{
    uint8_t head_canary[canary_size];
    uint8_t data[data_size];
    uint8_t tail_canary[canary_size];
}  __attribute__((packed));

static test_chunk chunk;

TEST_GROUP(memset_test)
{
    void setup()
    {
        // Fill head and tail canaries
        std::fill(chunk.head_canary, chunk.head_canary + canary_size, canary_byte);
        std::fill(chunk.tail_canary, chunk.tail_canary + canary_size, canary_byte);

        // Initialize data
        std::fill(chunk.data, chunk.data + data_size, initial_byte);
    }

    void teardown()
    {
        // Do nothing except canary validation
        CHECK(std::all_of(chunk.head_canary,
                          chunk.head_canary + canary_size,
                          [](auto elem){ return elem == canary_byte; }));
        CHECK(std::all_of(chunk.tail_canary,
                          chunk.tail_canary + canary_size,
                          [](auto elem){ return elem == canary_byte; }));
    }
};

TEST(memset_test, retval)
{
    POINTERS_EQUAL(chunk.data, our_memset(chunk.data, 0xff, 1));
}

TEST(memset_test, no_bytes_set)
{
    our_memset(chunk.data, test_byte, 0);
    std::for_each(chunk.data, chunk.data + data_size,
                  [](auto elem){ BYTES_EQUAL(initial_byte, elem); });
}

TEST(memset_test, all_bytes_set)
{
    our_memset(chunk.data, test_byte, data_size);
    std::for_each(chunk.data, chunk.data + data_size,
                  [](auto elem){ BYTES_EQUAL(test_byte, elem); });
}

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
