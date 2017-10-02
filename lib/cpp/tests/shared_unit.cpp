/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "memory.hpp"

#include <cstdlib> // For memory aligned allocator
#include <map>
#include <iostream>
#include <algorithm>

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

// Type of managable object.
struct dummy_type
{
    dummy_type(int a, double b, void *c)
        :a(a), b(b), c(c)
    {
        mock("dummy_type")
                .actualCall("dummy_type")
                .withParameter("a", a)
                .withParameter("b", b)
                .withParameter("c", c);
    }

    ~dummy_type() { mock("dummy_type").actualCall("~dummy_type"); }

    int a;
    double b;
    void *c;
};

// Holds status of particular memory chunk
struct chunk_info
{
    size_t size    = 0;
    bool   deleted = false;
};


// Tracks all memory allocations
static std::map< void*, chunk_info > allocations;

// Allocator with some additional checks
template< typename T >
struct dummy_allocator
{
    T* allocate(size_t n)
    {
        size_t size = n * sizeof(T);
        auto p = aligned_alloc(alignof(T), size);
        allocations.insert({p, { size, false }});
        return reinterpret_cast< T* >(p);
    }

    void deallocate(T *p, size_t n)
    {
        auto it = allocations.find(reinterpret_cast< void* >(p));

        // Invalid memory passed
        if (it == allocations.end()) {
            std::cout << "Given pointer: "
                      << p  << " wasn't previously allocated "
                      << std::endl;

            FAIL("Aborting test");
            return;
        }

        if (it->second.deleted) {
            std::cout << "Given pointer: "
                      << p  << " has been already deleted"
                      << std::endl;
            FAIL("Aborting test");
            return;
        }

        if (it->second.size != n * sizeof(T)) {
            std::cout << "Given pointer: "
                      << p  << " has been allocated with size: "
                      << it->second.size << " but requested to delete with "
                      << "different size: " << n * sizeof(T) << std::endl;
            FAIL("Aborting test");
            return;
        }

        it->second.deleted = true;
    }

    template< typename U >
    dummy_allocator< U > rebind() const
    {
        return dummy_allocator< U >{};
    }

    // Asserts that there is no allocations,
    void assert_all_deleted()
    {
        bool mem_leak = false;
        std::for_each(allocations.begin(), allocations.end(),
                      [&mem_leak](const auto &pair) {
            if (!pair.second.deleted) {
                std::cout << "Memory leak detected, pointer: "
                          << pair.first << '\n'
                          << "with size: " << pair.second.size
                          << " wasn't deleted";

                mem_leak = true;
            }
        });

        if (mem_leak) {
            FAIL("Memory leaks detected, aborting...");
        }
    }

    // Erases all recorded data
    void clear()
    {
        allocations.clear();
    }
};

static dummy_allocator< dummy_type > alloc;

//------------------------------------------------------------------------------
// Test group with tests dedicated to empty shared poitners and their
// interactions

TEST_GROUP(empty_shared)
{
    void setup() { }
    void teardown() { mock().clear(); }
};

TEST(empty_shared, default_constructed)
{
    ecl::shared_ptr< dummy_type > sp;

    // Unique always
    bool rc = sp.unique();
    CHECK_EQUAL(true, rc);

    // Doesn't own anything
    rc = !!sp;
    CHECK_EQUAL(false, rc);
    CHECK_FALSE(sp);

    // Holds nullptr internally
    auto ptr = sp.get();
    CHECK_TRUE(nullptr == ptr);

    // Comparsion routine sanity checks
    CHECK_TRUE(sp == nullptr);

    // Make sure no ctor\dtor was called
    mock("dummy_type").checkExpectations();
}

TEST(empty_shared, constructed_from_empty_pointer)
{
    ecl::shared_ptr< dummy_type > empty;

    ecl::shared_ptr< dummy_type > sp = empty;

    // Holds nullptr internally
    auto ptr = sp.get();
    CHECK_TRUE(nullptr == ptr);

    // Make sure that copy ctos didn't change anything
    ptr = empty.get();
    CHECK_TRUE(nullptr == ptr);

    // Make sure no ctor\dtor was called
    mock("dummy_type").checkExpectations();
}

TEST(empty_shared, moved_from_empty_poitner)
{
    ecl::shared_ptr< dummy_type > empty;

    ecl::shared_ptr< dummy_type > sp = std::move(empty);

    // Holds nullptr internally
    auto ptr = sp.get();
    CHECK_TRUE(nullptr == ptr);

    // Make sure that original shared pointer remains in default state
    ptr = empty.get();
    CHECK_TRUE(nullptr == ptr);

    // Make sure no ctor\dtor was called
    mock("dummy_type").checkExpectations();
}

TEST(empty_shared, assigned_empty_to_empty)
{
    ecl::shared_ptr< dummy_type > first_empty;
    ecl::shared_ptr< dummy_type > second_empty;

    second_empty = first_empty;

    // Holds nullptr internally
    auto ptr = second_empty.get();
    CHECK_TRUE(nullptr == ptr);

    // Make sure that original shared pointer remains in default state
    ptr = first_empty.get();
    CHECK_TRUE(nullptr == ptr);

    // Make sure no ctor\dtor was called
    mock("dummy_type").checkExpectations();
}


//------------------------------------------------------------------------------
//

TEST_GROUP(basic_shared)
{
    // Shared pointer allocated in heap in order to control
    // its ctors/dtos calls.
    ecl::shared_ptr< dummy_type > *test_ptr;

    int test_a = 10;
    double test_b = 100500;
    void *test_c = reinterpret_cast< void* >(&test_a);

    void setup()
    {
        test_ptr = new ecl::shared_ptr< dummy_type >;

        // Create managable object

        mock("dummy_type")
                .expectOneCall("dummy_type")
                .withParameter("a", test_a)
                .withParameter("b", test_b)
                .withParameter("c", test_c);

        *test_ptr = ecl::allocate_shared
                < dummy_type, dummy_allocator< dummy_type > >
                (alloc, test_a, test_b, test_c);
        mock().checkExpectations();

        // Check that allocations did really happen
        size_t size = allocations.size();
        CHECK_EQUAL(1, size);

        CHECK_EQUAL(test_a, (*test_ptr)->a);
        CHECK_EQUAL(test_b, (*test_ptr)->b);
        CHECK_EQUAL(test_c, (*test_ptr)->c);
    }

    void teardown()
    {
        // No one left
        bool rc = test_ptr->unique();
        CHECK_EQUAL(true, rc);

        // Object is about to be deleted
        if (test_ptr->get()) {
            mock("dummy_type").expectOneCall("~dummy_type");
        }

        delete test_ptr;

        // Check that there are no memory allocations left
        alloc.assert_all_deleted();

        alloc.clear();
        mock().checkExpectations();
        mock().clear();
    }
};

TEST(basic_shared, create_destroy)
{
    // Do nothing
}

TEST(basic_shared, copy_constructed)
{
    ecl::shared_ptr< dummy_type > other_ptr = *test_ptr;

    // Shared with existing ptr
    bool rc = other_ptr.unique();
    CHECK_EQUAL(false, rc);

    // Check pointer correctness

    auto p1 = other_ptr.get();
    auto p2 = test_ptr->get();

    CHECK_EQUAL(p1, p2);

    // Check for consistency trough gettters

    const auto &ref = *other_ptr;
    CHECK_EQUAL(&ref, p1);

    // Check for valid data

    CHECK_EQUAL(test_a, other_ptr->a);
    CHECK_EQUAL(test_b, other_ptr->b);
    CHECK_EQUAL(test_c, other_ptr->c);
}

TEST(basic_shared, assigned)
{
    mock("dummy_type").disable();
    ecl::shared_ptr< dummy_type > other_ptr = ecl::allocate_shared
            < dummy_type, dummy_allocator< dummy_type > >(alloc, 1, 2.0, nullptr);
    mock("dummy_type").enable();

    // Previous object must be destroyed
    mock("dummy_type").expectOneCall("~dummy_type");
    other_ptr = *test_ptr;
    mock().checkExpectations();

    // Shared with existing ptr
    bool rc = other_ptr.unique();
    CHECK_EQUAL(false, rc);

    // Check pointer correctness

    auto p1 = other_ptr.get();
    auto p2 = test_ptr->get();

    CHECK_EQUAL(p1, p2);

    // Check for valid data

    CHECK_EQUAL(test_a, other_ptr->a);
    CHECK_EQUAL(test_b, other_ptr->b);
    CHECK_EQUAL(test_c, other_ptr->c);
}

TEST(basic_shared, move_constructed)
{
    // For latter use
    auto p1 = test_ptr->get();

    ecl::shared_ptr< dummy_type > other_ptr = std::move(*test_ptr);

    // Test pointer must be unique now
    bool rc = test_ptr->unique();
    CHECK_EQUAL(true, rc);

    // Holds nullptr internally
    auto ptr = test_ptr->get();
    CHECK_TRUE(nullptr == ptr);

    // Check that ownership is transferred
    auto p2 = other_ptr.get();
    CHECK_EQUAL(p1, p2);

    // Check for valid data

    CHECK_EQUAL(test_a, other_ptr->a);
    CHECK_EQUAL(test_b, other_ptr->b);
    CHECK_EQUAL(test_c, other_ptr->c);

    // Dtor will be called at this point
    mock("dummy_type").expectOneCall("~dummy_type");
}

TEST(basic_shared, move_assigned)
{
    // For latter use
    auto p1 = test_ptr->get();

    mock("dummy_type").disable();
    ecl::shared_ptr< dummy_type > other_ptr = ecl::allocate_shared
            < dummy_type, dummy_allocator< dummy_type > >(alloc, 1, 2.0, nullptr);
    mock("dummy_type").enable();

    // Previous object must be destroyed
    mock("dummy_type").expectOneCall("~dummy_type");
    other_ptr = std::move(*test_ptr);
    mock().checkExpectations();

    // Test pointer must be unique now
    bool rc = test_ptr->unique();
    CHECK_EQUAL(true, rc);

    // Holds nullptr internally
    auto ptr = test_ptr->get();
    CHECK_TRUE(nullptr == ptr);

    // Check that ownership is transferred
    auto p2 = other_ptr.get();
    CHECK_EQUAL(p1, p2);

    // Check for valid data

    CHECK_EQUAL(test_a, other_ptr->a);
    CHECK_EQUAL(test_b, other_ptr->b);
    CHECK_EQUAL(test_c, other_ptr->c);

    // Dtor will be called at this point
    mock("dummy_type").expectOneCall("~dummy_type");
}

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
