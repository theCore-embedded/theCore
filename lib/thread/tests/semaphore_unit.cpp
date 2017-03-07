#include <ecl/thread/semaphore.hpp>

#include <array>
#include <thread>
#include <atomic>
#include <algorithm>
#include <functional>
#include <iostream>
#include <chrono>
#include <future>

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>

// Mocking timed-semaphores
volatile uint32_t mock_time = 0;

// TODO: remove ugly sleep-programming

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

// Time in semaphore timeout methods must be mocked in order to use timeout tests.
#ifdef SEMAPHORE_TEST_MOCKED_TIME
template<typename Sem>
static void test_timed_wait(bool signal)
{
    using namespace std::literals::chrono_literals;

    ecl::binary_semaphore to; // Sends signal to task

    // Async task
    auto task = [&to](auto to_wait) {
        // Wait for event
        return to.try_wait(to_wait);
    };

    // Initial time value will be '0', wait time - '1000'
    auto future = std::async(std::launch::async, task, 1000ms);

    // Let operation settle.
    test_delay(10);

    // Task should keep waiting for events.
    CHECK_TRUE(std::future_status::timeout == future.wait_for(0ms));

    if (signal) {
        // Progress time. Timeout should not be reached.
        mock_time = 500;

        // Signal semaphore
        to.signal();
    } else {
        // Progress time. Timeout should be reached.
        mock_time = 1001;
    }

    // Get result

    auto actual = future.get();
    CHECK_EQUAL(signal, actual);
}
#endif // SEMAPHORE_TEST_MOCKED_TIME

constexpr auto threads_count = 10;

TEST_GROUP(semaphore)
{
    void setup()
    {
    }

    void teardown()
    {
        mock_time = 0;
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
    CHECK_EQUAL(true, rc);

    rc = sem.try_wait();
    CHECK_EQUAL(false, rc);

    rc = sem.try_wait();
    CHECK_EQUAL(false, rc);

    // Signal same semaphore again
    // to make sure counter is in valid state
    sem.signal();

    rc = sem.try_wait();
    CHECK_EQUAL(true, rc);

    rc = sem.try_wait();
    CHECK_EQUAL(false, rc);
}

// Time in semaphore timeout methods must be mocked in order to use timeout tests.
#ifdef SEMAPHORE_TEST_MOCKED_TIME
TEST(semaphore, binary_timed_try_wait_timeout)
{
    test_timed_wait<ecl::binary_semaphore>(false);
}

TEST(semaphore, binary_timed_try_wait_succed)
{
    test_timed_wait<ecl::binary_semaphore>(true);
}

TEST(semaphore, timed_wait_timeout)
{
    test_timed_wait<ecl::semaphore>(false);
}

TEST(semaphore, timed_try_wait_succed)
{
    test_timed_wait<ecl::semaphore>(true);
}
#endif // SEMAPHORE_TEST_MOCKED_TIME

TEST(semaphore, one_semaphore_few_threads)
{
    // Preparation

    // Counts how many threads completed
    int counter = 0;
    // Counts how many signals sent
    int signalled = 0;

    // Semaphore itself
    ecl::semaphore entry_semaphore;
    ecl::semaphore exit_semaphore;

    auto single_thread = [&counter, &entry_semaphore, &exit_semaphore]() {
        std::cout << "entry wait!" << std::endl;

        bool ret;
        while ((ret = entry_semaphore.try_wait()) == false) {
            std::this_thread::yield();
        }

        counter++;
        exit_semaphore.signal();
    };

    std::array< std::thread, threads_count > threads;

    // Test itself

    // Start threads
    test_for_each(threads, [&single_thread](auto &thread) {
        thread = std::thread(single_thread);
    });

    // Let thread wait on semaphore
    test_delay(20);
    // Threads are started and should wait for orders
    CHECK_EQUAL(0, counter);
    // Unblock threads one by one
    test_for_each(threads, [&](auto &thread) {
        (void) thread; // We don't need this

        entry_semaphore.signal();
        signalled++;

        bool ret;
        while ((ret = exit_semaphore.try_wait()) == false) {
            std::this_thread::yield();
        }

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
    test_delay(50);
    // Check that work is done
    test_for_each(objs, [](auto &obj) { CHECK(obj.flag); });
    // Wait for threads to finish
    test_for_each(objs, [](auto &obj) { obj.thread.join(); });
}

//------------------------------------------------------------------------------

TEST_GROUP(binary_semaphore)
{
    void setup()
    {
    }

    void teardown()
    {
        mock_time = 0;
    }
};

TEST(binary_semaphore, try_wait)
{
    // Should complete without a hang
    ecl::binary_semaphore sem;

    sem.signal();

    auto rc = sem.try_wait();
    CHECK_EQUAL(true, rc);

    rc = sem.try_wait();
    CHECK_EQUAL(false, rc);

    rc = sem.try_wait();
    CHECK_EQUAL(false, rc);

    // Signal same semaphore again
    // to make sure counter is in valid state
    sem.signal();

    rc = sem.try_wait();
    CHECK_EQUAL(true, rc);

    rc = sem.try_wait();
    CHECK_EQUAL(false, rc);
}

TEST(binary_semaphore, more_than_one_signal)
{
    // Should complete without a hang and only one event must be reported
    ecl::binary_semaphore sem;

    sem.signal();
    sem.signal(); // Second should be 'ignored' as semaphore is already raised.

    sem.wait();

    auto rc = sem.try_wait();
    CHECK_EQUAL(false, rc);

    rc = sem.try_wait();
    CHECK_EQUAL(false, rc);

    // Signal same semaphore again
    // to make sure counter is in valid state
    sem.signal();
    sem.signal();

    rc = sem.try_wait();
    CHECK_EQUAL(true, rc);

    rc = sem.try_wait();
    CHECK_EQUAL(false, rc);
}


int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
