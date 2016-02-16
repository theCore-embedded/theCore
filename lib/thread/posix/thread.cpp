#include <ecl/thread/thread.hpp>
#include <ecl/assert.h>
#include <bits/local_lim.h>
#include <algorithm>

ecl::native_thread::native_thread()
    :m_thread{}
    ,m_stack{PTHREAD_STACK_MIN}
    ,m_name{""}
    ,m_state{state::initial}
    ,m_fn{nullptr}
    ,m_arg{nullptr}
{

}

ecl::native_thread::native_thread(native_thread &&other)
{
    this->m_thread = other.m_thread;
    this->m_state = other.m_state;
    this->m_fn = other.m_fn;
    this->m_stack = other.m_stack;
    this->m_arg = other.m_arg;
    this->m_name = std::move(other.m_name);

    other.m_state = state::initial;
}

ecl::native_thread::~native_thread()
{
    if (m_state == state::started) {
        pthread_detach(m_thread);
    }
}

ecl::err ecl::native_thread::set_stack_size(size_t size)
{
    ecl_assert_msg(size, "Stack size cannot be 0");

    if (m_state != state::initial) {
        return err::busy;
    }

    m_stack += size; // TODO: rationale for such values
    return err::ok;
}

ecl::err ecl::native_thread::set_name(const char *name)
{
    if (m_state == state::started) {
        int rc = pthread_setname_np(m_thread, m_name.c_str());
        if (rc != 0) {
            return err::generic;
        }

    } else if (m_state != state::initial) {
        return err::srch;
    }

    m_name = name;
    return err::ok;
}

ssize_t ecl::native_thread::get_name(char *buf, size_t size)
{
    size_t copied = m_name.copy(buf, size - 1);
    buf[copied] = '\0';
    return m_name.length();
}

ecl::err ecl::native_thread::set_routine(routine fn, void *arg)
{
    ecl_assert(fn);

    if (m_state != state::initial) {
        return err::busy;
    }

    m_arg = arg;
    m_fn = fn;

    return err::ok;
}

ecl::err ecl::native_thread::start()
{
    ecl_assert_msg(m_fn, "Attempting to start thread without thread routine");

    if (m_state != state::initial) {
        return err::busy;
    }

    pthread_attr_t attr;

    int rc = pthread_attr_init(&attr);

    if (rc != 0) {
        return err::generic;
    }

    // TODO: comment about why stack size is not used

    // TODO: comment about it
    runner_arg arg = { {}, m_fn, m_arg };

    rc = pthread_create(&m_thread, &attr, thread_runner, reinterpret_cast< void* >(&arg));
    if (rc != 0) {
        pthread_attr_destroy(&attr);
        return err::generic;
    }

    arg.start_flag.wait();

    m_state = state::started;

    rc = pthread_setname_np(m_thread, m_name.c_str());

    pthread_attr_destroy(&attr);

    return rc != 0 ? err::generic : err::ok;
}

ecl::err ecl::native_thread::join(ecl::err &retcode)
{
    void *retval;

    if (m_state != state::started) {
        return err::srch;
    }

    int rc = pthread_join(m_thread, &retval);
    if (rc != 0) {
        return err::generic;
    }

    retcode = * reinterpret_cast< ecl::err * >(&retval);

    m_state = state::initial;

    return err::ok;
}

ecl::err ecl::native_thread::join()
{
    ecl::err dummy;
    return join(dummy);
}

ecl::err ecl::native_thread::detach()
{
    if (m_state != state::started) {
        return err::srch;
    }

    int rc = pthread_detach(m_thread);
    if (rc != 0) {
        return err::generic;
    }

    // There is no return from detached state,
    // since there is no way to check if thread finish its execution or not
    m_state = state::detached;
    return err::ok;
}


//------------------------------------------------------------------------------

void* ecl::native_thread::thread_runner(void *arg)
{
    runner_arg *rarg = reinterpret_cast< runner_arg* >(arg);
    auto *fn = rarg->start_routine;
    auto *fn_arg = rarg->routine_arg;

    // TODO: comment
    rarg->start_flag.signal();

    err result = fn(fn_arg);
    return reinterpret_cast< void * >(result);
}

