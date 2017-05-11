#include <ecl/thread/thread.hpp>

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>

#include <cstring>
#include <thread>

//------------------------------------------------------------------------------
// Native thread in not started state tests

static int dummy_ctx = 0;

static ecl::err dummy_routine(void *arg)
{
    (void) arg;
    return ecl::err::ok;
}

static char name[16] = {0};
static ssize_t name_size;

TEST_GROUP(not_started_native_thread)
{
    void setup()
    {
        memset(name, 0, sizeof(name));
        name_size = 0;
    }

    void teardown()
    {
    }
};

TEST(not_started_native_thread, get_name_test)
{
    ecl::native_thread tr;

    name_size = tr.get_name(name, sizeof(name));
    CHECK_EQUAL(0, name_size);
    STRCMP_EQUAL("", name);
}

TEST(not_started_native_thread, set_name_test)
{
    ecl::native_thread tr;

    const char new_name[] = "cool_thread";
    auto rc = tr.set_name(new_name);
    CHECK_EQUAL(ecl::err::ok, rc);

    name_size = tr.get_name(name, sizeof(name));
    CHECK_EQUAL(sizeof(new_name) - 1, name_size);
    STRCMP_EQUAL(new_name, name);
}

TEST(not_started_native_thread, set_stack_size)
{
    ecl::native_thread tr;

    auto rc = tr.set_stack_size(1024);
    CHECK_EQUAL(ecl::err::ok, rc);
}

TEST(not_started_native_thread, set_routine)
{
    ecl::native_thread tr;

    auto rc = tr.set_routine(dummy_routine, nullptr);
    CHECK_EQUAL(ecl::err::ok, rc);

    rc = tr.set_routine(dummy_routine, &dummy_ctx);
    CHECK_EQUAL(ecl::err::ok, rc);
}

TEST(not_started_native_thread, join)
{
    ecl::native_thread tr;
    ecl::err dummy_retval;

    auto rc = tr.join(dummy_retval);
    CHECK_EQUAL(ecl::err::srch, rc);

    rc = tr.join();
    CHECK_EQUAL(ecl::err::srch, rc);
}

TEST(not_started_native_thread, detach)
{
    ecl::native_thread tr;

    auto rc = tr.detach();
    CHECK_EQUAL(ecl::err::srch, rc);
}

//------------------------------------------------------------------------------

struct test_arg
{
    ecl::semaphore      sem;
    bool                started;
    bool                completed;
    ecl::err            required_retval;
};

static test_arg routine_arg;

static ecl::err test_routine(void *arg)
{
    CHECK_EQUAL(&routine_arg, arg);
    test_arg *targ = reinterpret_cast< test_arg * >(arg);

    targ->started = true;
    targ->sem.wait();
    targ->completed = true;

    return targ->required_retval;
}

static ecl::native_thread started_tr;
static bool started_tr_joined_or_detached;

TEST_GROUP(started_native_thread)
{
    void setup()
    {
        memset(name, 0, sizeof(name));
        name_size = 0;

        // Set unusual error code
        routine_arg.required_retval = ecl::err::loop;
        // Thread shoulnd't be joined by default
        started_tr_joined_or_detached = false;

        auto rc = started_tr.set_routine(test_routine,
                                         reinterpret_cast< void* >(&routine_arg));
        CHECK_EQUAL(ecl::err::ok, rc);

        rc = started_tr.start();
        CHECK_EQUAL(ecl::err::ok, rc);

        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        CHECK_EQUAL(true, routine_arg.started);
        // Thread shouldn't complete by itself
        CHECK_EQUAL(false, routine_arg.completed);
    }

    void teardown()
    {
        if (!started_tr_joined_or_detached) {
            routine_arg.sem.signal();

            auto rc = started_tr.join();
            CHECK_EQUAL(ecl::err::ok, rc);
            CHECK_EQUAL(true, routine_arg.completed);
        }

        // Reset name and state
        routine_arg.started = false;
        routine_arg.completed = false;
        started_tr.set_name("");
    }
};

TEST(started_native_thread, basic)
{
    // Thread in this test just starts and finishes.
    // See setup()/teardown() section
}

TEST(started_native_thread, get_name_test)
{
    name_size = started_tr.get_name(name, sizeof(name));
    CHECK_EQUAL(0, name_size);
    STRCMP_EQUAL("", name);
}

TEST(started_native_thread, set_name_test)
{
    const char new_name[] = "cool_thread";
    auto rc = started_tr.set_name(new_name);
    CHECK_EQUAL(ecl::err::ok, rc);

    name_size = started_tr.get_name(name, sizeof(name));
    CHECK_EQUAL(sizeof(new_name) - 1, name_size);
    STRCMP_EQUAL(new_name, name);
}

TEST(started_native_thread, join_with_parameter)
{
    routine_arg.sem.signal();

    ecl::err thread_retval;

    auto rc = started_tr.join(thread_retval);
    CHECK_EQUAL(ecl::err::ok, rc);
    CHECK_EQUAL(true, routine_arg.completed);
    CHECK_EQUAL(routine_arg.required_retval, thread_retval);

    // Notify shutdown routine that thread already joined
    started_tr_joined_or_detached = true;
}

//------------------------------------------------------------------------------

// TODO: state of detached thread cannot be restored back to the initial state
// more tests in detached mode will require additional routine that
// will return new thread or allocate new thread in heap and delete it
// after test finishes
static ecl::native_thread detached_tr;

TEST_GROUP(detached_native_thread)
{
    void setup()
    {
        auto rc = detached_tr.set_routine(test_routine,
                                          reinterpret_cast< void* >(&routine_arg));
        CHECK_EQUAL(ecl::err::ok, rc);

        rc = detached_tr.start();
        CHECK_EQUAL(ecl::err::ok, rc);

        // TODO: might be useful to add here some delay

        CHECK_EQUAL(true, routine_arg.started);
        // Thread shouldn't complete by itself
        CHECK_EQUAL(false, routine_arg.completed);

        rc = detached_tr.detach();
        CHECK_EQUAL(ecl::err::ok, rc);
    }

    void teardown()
    {
        // Reset state
        routine_arg.started = false;
        routine_arg.completed = false;
    }
};

TEST(detached_native_thread, basic)
{
    // Let thread go
    routine_arg.sem.signal();

    // Some time is requried to let thread finish
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    CHECK_EQUAL(true, routine_arg.completed);
}

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
