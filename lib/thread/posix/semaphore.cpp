#include <ecl/thread/semaphore.hpp>

ecl::semaphore::semaphore()
    :m_mutex{}
    ,m_cond{}
    ,m_flag{false}
{
}

void ecl::semaphore::signal()
{
    std::unique_lock< std::mutex > lock(m_mutex);
    m_flag = true;
    lock.unlock();
    m_cond.notify_one();
}

void ecl::semaphore::wait()
{
    std::unique_lock< std::mutex > lock(m_mutex);
    m_cond.wait(lock, [&]{ return m_flag.load(); }); // To avoid spurious wakeup
    m_flag = false;
}

