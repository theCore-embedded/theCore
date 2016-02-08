#ifndef LIB_THREAD_HOST_SEMAPHORE_HPP_
#define LIB_THREAD_HOST_SEMAPHORE_HPP_

#include <condition_variable>
#include <atomic>

namespace ecl
{

class semaphore
{
public:
    semaphore();

    void signal();
    void wait();

    semaphore(const semaphore&)             = delete;
    semaphore& operator=(const semaphore&)  = delete;

private:
    std::mutex                  m_mutex;
    std::condition_variable     m_cond;
    std::atomic_bool            m_flag;
};

}

#endif
