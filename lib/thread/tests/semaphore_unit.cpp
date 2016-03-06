#include <ecl/thread/semaphore.hpp>

#include <array>
#include <thread>
#include <atomic>
#include <algorithm>
#include <functional>

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>

// Error code helper
// TODO: move it to 'utils' headers and protect with check of
// current test state (enabled or disabled)
static SimpleString StringFrom(ecl::err err)
{
    return SimpleString{ecl::err_to_str(err)};
}

// Helpers
template< class Container, class Func >
static void test_for_each(Container &cont, Func fn)
{
    std::for_each(cont.begin(), cont.end(), fn);
}

static void test_delay(unsigned msecs = 1000)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(msecs));
}

constexpr auto threads_count = 10;

TEST_GROUP(semaphore)
{
    void setup()
    {
    }

    void teardown()
    {
    }
};

TEST(semaphore, signal_wait_no_hang)
{
    // Should complete without a hang
    ecl::semaphore sem;
    sem.signal();
    sem.wait();
}

TEST(semaphore, try_wait)
{
    // Should complete without a hang
    ecl::semaphore sem;

    sem.signal();

    auto rc = sem.try_wait();
    CHECK_EQUAL(ecl::err::ok, rc);

    rc = sem.try_wait();
    CHECK_EQUAL(ecl::err::again, rc);

    rc = sem.try_wait();
    CHECK_EQUAL(ecl::err::again, rc);

    // Signal same semaphore again
    // to make sure counter is in valid state
    sem.signal();

    rc = sem.try_wait();
    CHECK_EQUAL(ecl::err::ok, rc);

    rc = sem.try_wait();
    CHECK_EQUAL(ecl::err::again, rc);
}

TEST(semaphore, one_semaphore_few_threads)
{
    // Preparation

    // Counts how many threads completed
    int counter = 0;
    // Counts how many signals sent
    int signalled = 0;

    // Semaphore itself
    ecl::semaphore semaphore;

    auto single_thread = [&counter, &semaphore]() {
        semaphore.wait();
        counter++;
    };

    std::array< std::thread, threads_count > threads;

    // Test itself

    // Start threads
    test_for_each(threads, [&single_thread](auto &thread) {
        thread = std::thread(single_thread);
    });

    // Let them wait on semaphore
    test_delay(20);
    // Threads are started and should wait for orders
    CHECK_EQUAL(0, counter);
    // Unblock threads one by one
    test_for_each(threads, [&](auto &thread) {
        (void) thread; // We don't need this
        semaphore.signal();
        signalled++;
        test_delay(50); // Let some thread finish its work
        CHECK_EQUAL(signalled, counter); // Check that only one thread is finished
    });

    // Wait for threads to finish
    test_for_each(threads, [](auto &thread) { thread.join(); });
}

TEST(semaphore, multiple_threads)
{
    // Preparation

    // Object associated with a thread
    struct test_object
    {
        ecl::semaphore      semaphore;
        std::thread         thread;
        volatile bool       flag;
    };

    std::array< test_object, threads_count > objs;

    auto single_thread = [](auto *obj) {
        obj->semaphore.wait();
        obj->flag = true; // Rise a flag only if semaphore signaled
    };

    // Test itself

    // Start threads
    test_for_each(objs, [&single_thread](auto &obj) {
        obj.flag = false;
        obj.thread = std::thread(single_thread, &obj);
    });

    // Let them wait on semaphore
    test_delay(100);
    // Threads are started and should wait for orders
    test_for_each(objs, [](auto &obj) { CHECK(!obj.flag); } );
    // Unblock threads
    test_for_each(objs, [](auto &obj) { obj.semaphore.signal(); });
    // Let threads do the work
    test_delay(100);
    // Check that work is done
    test_for_each(objs, [](auto &obj) { CHECK(obj.flag); });
    // Wait for threads to finish
    test_for_each(objs, [](auto &obj) { obj.thread.join(); });
}

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
