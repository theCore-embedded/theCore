/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ecl/assert.h>
#include <ecl/thread/thread.hpp>
#include <string.h>

#include <algorithm>

ecl::native_thread::native_thread()
    :m_task{nullptr}
    ,m_stack{512} // TODO: rationale, why this number?
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

ecl::native_thread::~native_thread()
{
    // TODO: comment
    this->m_state = state::detached;
}

ecl::err ecl::native_thread::set_stack_size(size_t size)
{
    ecl_assert_msg(size, "Stack size cannot be 0");

    if (m_state != state::initial) {
        return err::busy;
    }

    m_stack = size;
    return err::ok;
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

    // TODO: comment about it
    runner_arg arg = { false, m_fn, m_arg };

    xTaskCreate(thread_runner,
                m_name,
                m_stack / sizeof(StackType_t),
                reinterpret_cast< void* >(&arg),
                tskIDLE_PRIORITY,
                &m_task);

    if (!m_task) {
        return ecl::err::generic;
    }


    while (!arg.start_flag) {
        // yield-spinlock
        taskYIELD();
    }

    m_state = state::detached;

    return ecl::err::ok;
}


ecl::err ecl::native_thread::join(ecl::err &retcode)
{
    (void) retcode;
    ecl_assert_msg(0, "Thread join is not supported on FreeRTOS (yet)");
    return ecl::err::notsup;
}

ecl::err ecl::native_thread::join()
{
    ecl::err dummy;
    return join(dummy);
}

//------------------------------------------------------------------------------

void ecl::native_thread::thread_runner(void *arg)
{
    runner_arg *rarg = reinterpret_cast< runner_arg* >(arg);
    auto *fn = rarg->start_routine;
    auto *fn_arg = rarg->routine_arg;

    // TODO: comment
    rarg->start_flag = true;

    fn(fn_arg);

    // According to the FreeRTOS convention, task that wants exit, must `delete`
    // itself explicitly.
    // Task deletion is a complex process though and FreeRTOS may not support it
    // under certain configuration. For example,  memory management that isn't
    // capable of memory will abort if someone will try to delete memory associated
    // with a task.
    vTaskDelete(NULL);
}

