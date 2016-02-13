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
