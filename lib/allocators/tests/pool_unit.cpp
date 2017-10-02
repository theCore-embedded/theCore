/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#define POOL_ALLOC_TEST_PRINT_STATS
#define POOL_ALLOC_TEST

#include <ecl/pool.hpp>

#include <sstream>
#include <algorithm>
#include <bitset>
#include <cstdlib>
#include <ctime>
#include <map>
#include <iostream>

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

// TODO: extend this suite with tests for invalid input
// This will be possible if

static constexpr auto block_size    = 16; // Must be power of two
static constexpr auto blocks        = 100;

static ecl::pool_base                   *test_pool;
static ecl::pool< block_size, blocks >  *real_pool;

// Represents a type that fits in exacly one block
struct exactly_block
{
    char data[block_size];
} __attribute__((packed));

// Represents a type that has a size greather than block, but less than two blocks
struct more_than_block
{
    char data[block_size + block_size / 3];
} __attribute__((packed));

// Represents a type that has a size less than a block
struct less_than_block
{
    char data[block_size - block_size / 4];
} __attribute__((packed));

// Makes sure that nothing present in empty pool.
// Must be called against empty pool.
static void check_if_empty_pool_is_empty()
{
    auto &info = real_pool->get_info();
    for (size_t i = 0; i < info.size(); i++) {
        auto byte = info[i];
        if (byte) { // Something is still present in pool
            std::stringstream ss;
            ss << "Byte at position [" << i << "] indicates that something is "
               << "present inside pool\n";
            real_pool->print_stats();
            FAIL(ss.str().c_str());
        }
    }
}

TEST_GROUP(pool_unit)
{
    void setup()
    {
        test_pool = real_pool = new ecl::pool< block_size, blocks >;
    }

    void teardown()
    {
        delete test_pool;
    }
};


// Tests allocation and check how many blocks are used.
template< typename T >
static void test_alloc_dealloc_with_type(size_t n, size_t blk_cnt)
{
    auto &info = real_pool->get_info();
    auto p = test_pool->aligned_alloc< T >(n);

    // Check alingment requiremets.
    auto p_val = reinterpret_cast< uintptr_t >(p);
    CHECK_EQUAL(0, p_val & (alignof(T) - 1));

    size_t cnt = 0;

    // Count how many blocks are used.
    std::for_each(info.begin(), info.end(), [&cnt](auto val) {
        cnt += std::bitset< 8 >{val}.count();
    });

    if (blk_cnt != cnt) {
        std::cout << "\n\nRequested type size: " << sizeof(T) << std::endl;
        std::cout << "Requested allocation size: " << n << std::endl;
        std::cout << "Summary (just before test ends):\n\n" << std::endl;
        real_pool->print_stats();
        // Express a fail
        CHECK_EQUAL(blk_cnt, cnt);
    }

    test_pool->deallocate< T >(p, n);

    // After end of this test, pool must be empty
    check_if_empty_pool_is_empty();
}

TEST(pool_unit, multiple_alloc_dealloc_of_block_type)
{
    for (size_t i = 1; i <= blocks; ++i) {
        test_alloc_dealloc_with_type< exactly_block >(i, i);
    }
}

TEST(pool_unit, too_big_allocation)
{
    auto p = test_pool->aligned_alloc< exactly_block >(blocks + 10);
    POINTERS_EQUAL(nullptr, p);

    // After end of this test, pool must be empty
    check_if_empty_pool_is_empty();
}

TEST(pool_unit, depleted_pool)
{
    auto p = test_pool->aligned_alloc< exactly_block >(blocks);
    CHECK_TRUE(p != nullptr);

    // No memory must be present in the pool.
    p = test_pool->aligned_alloc< exactly_block >(1);
    POINTERS_EQUAL(nullptr, p);
}

TEST(pool_unit, single_alloc_dealloc)
{
    test_alloc_dealloc_with_type< exactly_block >(1, 1);
    test_alloc_dealloc_with_type< less_than_block >(1, 1);
    // Will occupy two blocks. See a size of a 'more_than_block' type.
    test_alloc_dealloc_with_type< more_than_block >(1, 2);
}

TEST(pool_unit, randomly_allocate_and_deallocate)
{
    // This test must randomize input patern of allocation sizes and types
    // and see whan happens.

    auto seed = std::time(0);
    std::srand(seed);

    // This will help reproduce a case if test fails.
    std::cout << ">>>>>> Seed is: " << seed << " <<<<<<\n";

    bool space_left = true;

    // Maps to keep track of allocations
    std::map< less_than_block*, size_t > less;
    std::map< exactly_block*,   size_t > exact;
    std::map< more_than_block*, size_t > more;

    while (space_left) {
        int op      = std::rand() % 3;
        size_t cnt  = std::rand() % 5 + 1;

        // TODO: when generic lambdas will be ready, use it
        // instead of this copy-pase horror
        switch (op) {
        case 0: {
            // Allocate less than block type
            auto p = test_pool->aligned_alloc< less_than_block >(cnt);

            if (!p) {
                space_left = false;
            } else {
                less.insert({p, cnt});
            }

            break;
        }
        case 1: {
            // Allocate exact block
            auto p = test_pool->aligned_alloc< exactly_block >(cnt);

            if (!p) {
                space_left = false;
            } else {
                exact.insert({p, cnt});
            }

            break;
        }
        case 2: {
            // Allocate more than one block
            auto p = test_pool->aligned_alloc< more_than_block >(cnt);

            if (!p) {
                space_left = false;
            } else {
                more.insert({p, cnt});
            }

            break;
        }
        }
    }

    // Now deallocate back all resourses

    std::for_each(less.begin(), less.end(), [=](auto item) {
        test_pool->deallocate< less_than_block >(item.first, item.second);
    });

    std::for_each(exact.begin(), exact.end(), [=](auto item) {
        test_pool->deallocate< exactly_block >(item.first, item.second);
    });

    std::for_each(more.begin(), more.end(), [=](auto item) {
        test_pool->deallocate< more_than_block >(item.first, item.second);
    });

    // Check that no data is present
    check_if_empty_pool_is_empty();
}
