//! \file
//! \brief Implementation of the semaphore for targets without OS support.

#include <ecl/thread/semaphore.hpp>
#include <common/execution.h>
#include <common/time.hpp>

void ecl::semaphore::signal()
{
    m_counter++;
}

void ecl::semaphore::wait()
{
    int cnt;

    if ((cnt = m_counter.fetch_sub(1)) <= 0) {
        while (m_counter.load() < cnt) { }
    }
}

bool ecl::semaphore::try_wait(std::chrono::milliseconds ms)
{
    // Some platform support not yet include RTC.
    // Time reference is possibly provided by simple clock counting, which
    // can be wrapped.
    auto now = std::chrono::milliseconds(0);

    int cnt;
    bool exch = false;

    while (((cnt = m_counter.load()) <= 0   // Wait till counter goes up enough
                                            // to avoid block.
               // After counter uprised, try to set new value.
               // Magic here is that after counter is checked new thread can
               // decrement it. Compare-and-exchange makes sure that counter
               // is not modified by other threads.
               || !(exch = m_counter.compare_exchange_weak(cnt, cnt - 1)))
           // Keep track of timeout.
           && now++ < ms)
    {
        ecl_spin_wait(1);
    }

    return exch;
}

//------------------------------------------------------------------------------

void ecl::binary_semaphore::signal()
{
    m_flag = true;
}

void ecl::binary_semaphore::wait()
{
    while (!m_flag);
    m_flag = false; // Semaphore taken.
}

bool ecl::binary_semaphore::try_wait(std::chrono::milliseconds ms)
{
    // Some platform support not yet include RTC.
    // Time reference is possibly provided by simple clock counting, which
    // can be wrapped.
    auto now = std::chrono::milliseconds(0);

    bool ex;

    while (!(ex = m_flag.exchange(false)) && now++ < ms) {
        ecl_spin_wait(1);
    }

    // If last exchanged value is false, it means that timeout was reached.
    return ex;
}
