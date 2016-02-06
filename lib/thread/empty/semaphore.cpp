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
