// Empty semaphore, acting like spinlock

#include <ecl/thread/semaphore.hpp>

constexpr ecl::semaphore::semaphore()
    :m_lock{0}
{ }

void ecl::semaphore::signal()
{
    m_lock.unlock();
}

void ecl::semaphore::wait()
{
    m_lock.lock();
}
