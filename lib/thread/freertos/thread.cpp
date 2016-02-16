#include <ecl/assert.h>
#include <ecl/thread/thread.hpp>
#include <string.h>

#include <algorithm>

ecl::native_thread::native_thread()
    :m_task{nullptr}
    ,m_stack{128}
    ,m_name{0}
    ,m_state{state::initial}
    ,m_fn{nullptr}
    ,m_arg{nullptr}
{
}

ecl::native_thread::native_thread(native_thread &&other)
    :m_task{other.m_task}
    ,m_stack{other.m_stack}
    ,m_name{0}
    ,m_state{other.m_state}
    ,m_fn{other.m_fn}
    ,m_arg{other.m_arg}
{
    memcpy(this->m_name, other.m_name, sizeof(m_name));

    other.m_state = state::detached;
}

ecl::err ecl::native_thread::set_name(const char *name)
{
    // TODO: substitude with strncpy or strlcpy ?
    size_t len = strlen(name);
    if (len >= sizeof(configMAX_TASK_NAME_LEN)) {
        return ecl::err::nobufs;
    }

    // Copy null-terminator as well
    std::copy(name, name + len + 1, m_name);
    return ecl::err::ok;
}

ssize_t ecl::native_thread::get_name(char *buf, size_t size)
{
    size_t len = strlen(m_name);
    size_t dest_len = size - 1; // Reserve space for null-terminator
    size_t to_copy = std::min(len, dest_len);

    std::copy(m_name, m_name + to_copy, buf);
    buf[to_copy] = '\0';

    return len;
}

ecl::err ecl::native_thread::set_routine(routine fn, void *arg)
{
    ecl_assert(fn);

    if (m_state != state::initial) {
        return ecl::err::busy;
    }

    m_arg = arg;
    m_fn = fn;

    return ecl::err::ok;
}

ecl::err ecl::native_thread::start()
{
    ecl_assert_msg(m_fn, "Attempting to start thread without thread routine");

    if (m_state != state::initial) {
        return ecl::err::busy;
    }

#if 0
    pthread_attr_t attr;

    int rc = pthread_attr_init(&attr);

    if (rc != 0) {
        return err::generic;
    }
#endif

    // TODO: comment about why stack size is not used

    // TODO: comment about it
    runner_arg arg = { {}, m_fn, m_arg };

    xTaskCreate(thread_runner,
                m_name,
                m_stack / sizeof(StackType_t),
                reinterpret_cast< void* >(&arg),
                tskIDLE_PRIORITY,
                &m_task);

    if (!m_task) {
        return ecl::err::generic;
    }

#if 0
    rc = pthread_create(&m_thread, &attr, thread_runner, reinterpret_cast< void* >(&arg));
    if (rc != 0) {
        pthread_attr_destroy(&attr);
        return err::generic;
    }
#endif

    arg.start_flag.wait();

    m_state = state::started;

#if 0
    rc = pthread_setname_np(m_thread, m_name.c_str());

    pthread_attr_destroy(&attr);
#endif

    return ecl::err::ok;
}


//------------------------------------------------------------------------------

void ecl::native_thread::thread_runner(void *arg)
{
    (void) arg;
}

