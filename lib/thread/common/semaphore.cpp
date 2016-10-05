// Empty semaphore

#include <ecl/thread/common/semaphore.hpp>

void ecl::common::semaphore::signal()
{
    m_counter++;
}

void ecl::common::semaphore::wait()
{
    int cnt;

    if ((cnt = m_counter.fetch_sub(1)) <= 0) {
        while (m_counter.load() < cnt) { }
    }
}

bool ecl::common::semaphore::try_wait()
{
    bool rc = true;

    if (m_counter.fetch_sub(1) <= 0) {
        m_counter++;
        rc = false;
    }

    return rc;
}

//------------------------------------------------------------------------------

void ecl::common::binary_semaphore::signal()
{
    m_flag = true;
}

void ecl::common::binary_semaphore::wait()
{
    while (!m_flag);
}

bool ecl::common::binary_semaphore::try_wait()
{
    if (m_flag.exchange(false)) {
        return true;
    }

    return false;
}
