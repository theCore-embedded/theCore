#include <unity.h>
#include <unity_fixture.h>

#include <ecl/thread/semaphore.hpp>

static ecl::binary_semaphore bsm;

TEST_GROUP(binary_semaphore);

TEST_SETUP(binary_semaphore) { }

TEST_TEAR_DOWN(binary_semaphore) { }

TEST(binary_semaphore, signal_wait)
{
    // Should not block
    bsm.signal();
    bsm.wait();
}

TEST(binary_semaphore, signal_try_wait)
{
    // Should not block
    bool rc = bsm.try_wait();
    TEST_ASSERT_FALSE(rc);

    rc = bsm.try_wait(std::chrono::milliseconds(5000));
    TEST_ASSERT_FALSE(rc);

    bsm.signal();

    rc = bsm.try_wait(std::chrono::milliseconds(5000));
    TEST_ASSERT_TRUE(rc);

    rc = bsm.try_wait();
    TEST_ASSERT_FALSE(rc);

    rc = bsm.try_wait(std::chrono::milliseconds(5000));
    TEST_ASSERT_FALSE(rc);
}

//------------------------------------------------------------------------------

static ecl::semaphore sm;

TEST_GROUP(semaphore);

TEST_SETUP(semaphore) { }

TEST_TEAR_DOWN(semaphore) { }

TEST(semaphore, signal_wait)
{
    // Should not block
    sm.signal();
    sm.signal();
    sm.signal();

    sm.wait();
    sm.wait();
    sm.wait();
}

TEST(semaphore, signal_try_wait)
{
    // Should not block
    bool rc = sm.try_wait();
    TEST_ASSERT_FALSE(rc);

    rc = sm.try_wait(std::chrono::milliseconds(5000));
    TEST_ASSERT_FALSE(rc);

    sm.signal();

    rc = sm.try_wait(std::chrono::milliseconds(5000));
    TEST_ASSERT_TRUE(rc);

    rc = sm.try_wait();
    TEST_ASSERT_FALSE(rc);

    rc = sm.try_wait(std::chrono::milliseconds(5000));
    TEST_ASSERT_FALSE(rc);
}
