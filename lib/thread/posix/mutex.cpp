#include <ecl/thread/mutex.hpp>

constexpr ecl::mutex::mutex()
    :m_mutex{}
{
}

void ecl::mutex::lock()
{
    m_mutex.lock();
}

void ecl::mutex::unlock()
{
    m_mutex.unlock();
}

bool ecl::mutex::try_lock()
{
    return m_mutex.try_lock();
}
