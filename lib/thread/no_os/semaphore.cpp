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

bool ecl::semaphore::try_wait()
{
    bool rc = true;

    if (m_counter.fetch_sub(1) <= 0) {
        m_counter++;
        rc = false;
    }

    return rc;
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
