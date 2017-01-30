//! \file
//! \brief Implementation of the semaphore for targets without OS support.

#include <ecl/thread/semaphore.hpp>
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
    auto till = get_ms_time() + ms;

    int cnt;
    bool exch = false;

    // Will block.
    while (((cnt = m_counter.load()) <= 0   // Wait till counter goes up enough
                                            // to avoid block.
               // After counter uprised, try to set new value.
               // Magic here is that after counter is checked new thread can
               // decrement it. Compare-and-exchange makes sure that counter
               // is not modified by other threads.
               || !(exch = m_counter.compare_exchange_weak(cnt, cnt - 1)))
           // Keep track of timeout.
           && get_ms_time() < till)
    { }

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
}

bool ecl::binary_semaphore::try_wait(std::chrono::milliseconds ms)
{
    auto till = get_ms_time() + ms;

    bool ex;

    while (!(ex = m_flag.exchange(false)) && get_ms_time() < till) {

    }

    // If last exchanged value is false, it means that timeout was reached.
    return ex;
}
