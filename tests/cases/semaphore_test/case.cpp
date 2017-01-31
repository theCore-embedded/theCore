#include <unity.h>
#include <unity_fixture.h>

#include <ecl/thread/semaphore.hpp>

TEST_GROUP(semaphore);

TEST_SETUP(semaphore) { }

TEST_TEAR_DOWN(semaphore) { }

TEST(semaphore, signal_wait)
{
    // Should not block
    ecl::binary_semaphore sm;

    sm.signal();
    sm.wait();
}
