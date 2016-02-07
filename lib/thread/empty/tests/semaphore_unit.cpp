#include <ecl/thread/semaphore.hpp>

#include <array>
#include <thread>
#include <atomic>
#include <algorithm>

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>

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

TEST(semaphore, multiple_threads)
{
    constexpr auto threads_count = 10;

    // Object associated with a thread
    struct test_object
    {
        ecl::semaphore      semaphore;
        std::thread         thread;
        volatile bool       flag;
    };

    std::array< test_object, threads_count > objs;

    // Preparation

    auto single_thread = [](auto *obj) {
        obj->semaphore.wait();
        obj->flag = true; // Rise a flag only if semaphore signaled
    };

    auto test_delay = []() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    };

    auto test_iterate = [&objs](auto fn) {
        std::for_each(objs.begin(), objs.end(), fn);
    };

    // Test itself

    // Start threads
    test_iterate([&single_thread](auto &obj) {
        obj.flag = false;
        obj.thread = std::thread(single_thread, &obj);
    });

    // Let them wait on semaphore
    test_delay();
    // Threads are started and should wait for orders
    test_iterate([](auto &obj) { CHECK(!obj.flag); } );
    // Unblock threads
    test_iterate([](auto &obj) { obj.semaphore.signal(); });
    // Let threads do the work
    test_delay();
    // Check that work is done
    test_iterate([](auto &obj) { CHECK(obj.flag); });
    // Wait for threads to finish
    test_iterate([](auto &obj) { obj.thread.join(); });
}

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
