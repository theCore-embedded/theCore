// Empty semaphore

#include <ecl/thread/semaphore.hpp>

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

bool ecl::binary_semaphore::try_wait()
{
    if (m_flag.exchange(false)) {
        return true;
    }

    return false;
}
