#include <ecl/thread/thread.hpp>

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>

#include <cstring>

// Error code helper
// TODO: move it to 'utils' headers and protect with check of
// current test state (enabled or disabled)
static SimpleString StringFrom(ecl::err err)
{
    return SimpleString{ecl::err_to_str(err)};
}

//------------------------------------------------------------------------------
// Native thread in not started state tests

static int dummy_ctx = 0;

static ecl::err dummy_routine(void *arg)
{
    (void) arg;
    return ecl::err::ok;
}

TEST_GROUP(not_started_native_thread)
{
    char name[16] = {0};
    ssize_t name_size;


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
static bool started_tr_joined;

TEST_GROUP(started_native_thread)
{
    void setup()
    {
        // Set unusual error code
        routine_arg.required_retval = ecl::err::loop;
        // Thread shoulnd't be joined by default
        started_tr_joined = false;

        auto rc = started_tr.set_routine(test_routine,
                                         reinterpret_cast< void* >(&routine_arg));
        CHECK_EQUAL(ecl::err::ok, rc);

        rc = started_tr.start();
        CHECK_EQUAL(ecl::err::ok, rc);

        CHECK_EQUAL(true, routine_arg.started);
        // Thread shouldn't complete by itself
        CHECK_EQUAL(false, routine_arg.completed);
    }

    void teardown()
    {
        if (!started_tr_joined) {
            routine_arg.sem.signal();

            auto rc = started_tr.join();
            CHECK_EQUAL(ecl::err::ok, rc);
            CHECK_EQUAL(true, routine_arg.completed);
        }

    }
};

TEST(started_native_thread, detach)
{
}


int main(int argc, char *argv[])
{
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
