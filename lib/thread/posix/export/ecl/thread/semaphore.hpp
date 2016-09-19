#ifndef LIB_THREAD_HOST_SEMAPHORE_HPP_
#define LIB_THREAD_HOST_SEMAPHORE_HPP_

#include <condition_variable>
#include <atomic>
#include <ecl/err.hpp>

//------------------------------------------------------------------------------

namespace ecl
{

class semaphore
{
public:
    semaphore();

    void signal();
    void wait();
    ecl::err try_wait();

    semaphore(const semaphore&)             = delete;
    semaphore& operator=(const semaphore&)  = delete;

private:
    std::mutex                  m_mutex;
    std::condition_variable     m_cond;
    std::atomic_int             m_cnt;
};

//------------------------------------------------------------------------------

class binary_semaphore
{
public:
    binary_semaphore();

    void signal();
    void wait();
    ecl::err try_wait();

    binary_semaphore(const binary_semaphore&)             = delete;
    binary_semaphore& operator=(const binary_semaphore&)  = delete;

private:
    std::mutex                  m_mutex;
    std::condition_variable     m_cond;
    std::atomic_bool            m_flag;
};

} // namespace ecl

#endif // LIB_THREAD_HOST_SEMAPHORE_HPP_
