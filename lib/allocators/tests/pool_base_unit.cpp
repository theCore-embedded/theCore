#include <ecl/pool.hpp>

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>


// Our pool mock class
class pool_mock : public ecl::pool_base
{
public:
    pool_mock() { }
    ~pool_mock() { }

    uint8_t *real_alloc(size_t n, size_t align, size_t obj_sz) override
    {
        mock("pool")
                .actualCall("real_alloc")
                .withLongIntParameter("count", n)
                .withLongIntParameter("align", align)
                .withLongIntParameter("size", obj_sz);


        auto *ret = mock("pool").returnPointerValueOrDefault(nullptr);
        return reinterpret_cast< uint8_t* >(ret);
    }

    void real_dealloc(uint8_t *p, size_t n, size_t obj_sz) override
    {
        mock("pool")
                .actualCall("real_dealloc")
                .withPointerParameter("ptr", p)
                .withLongIntParameter("count", n)
                .withLongIntParameter("size", obj_sz);
    }
};

// An object struct with specific alingment and size requirements
struct test_type
{
    char    y;
    int     x;
    double  r;
};

// Test object itself
static test_type obj;

ecl::pool_base *test_pool;

TEST_GROUP(pool_base)
{
    void setup()
    {
        test_pool = new pool_mock;
    }

    void teardown()
    {
        delete test_pool;
        mock().clear();
    }
};

TEST(pool_base, aligned_alloc_type_requirements)
{

    uint8_t *ptr = reinterpret_cast< uint8_t* >(&obj);
    long count = 10;
    long align = alignof(test_type);
    long size  = sizeof(obj);

    mock("pool")
            .expectOneCall("real_alloc")
            .withLongIntParameter("count", count)
            .withLongIntParameter("align", align)
            .withLongIntParameter("size", size)
            .andReturnValue(ptr);

    auto *ret = test_pool->template aligned_alloc< test_type >(count);

    mock().checkExpectations();
    CHECK_EQUAL(ptr, reinterpret_cast< uint8_t* >(ret));
}

TEST(pool_base, aligned_dealloc_type_requirements)
{
    uint8_t *ptr = reinterpret_cast< uint8_t* >(&obj);
    long count = 25;
    long size  = sizeof(obj);

    mock("pool")
            .expectOneCall("real_dealloc")
            .withPointerParameter("ptr", ptr)
            .withLongIntParameter("count", count)
            .withLongIntParameter("size", size);

    test_pool->template deallocate< test_type >(&obj, count);

    mock().checkExpectations();
}
