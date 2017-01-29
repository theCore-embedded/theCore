//! \file
//! \brief Semaphore implementation for projects without OS support.

#ifndef LIB_THREAD_NO_OS_SEMAPHORE_
#define LIB_THREAD_NO_OS_SEMAPHORE_

#include <ecl/err.hpp>

#include <atomic>

namespace ecl
{

// Empty semaphore, acting like spinlock of some kind
class semaphore
{
public:
    constexpr semaphore()
        :m_counter{0} { }

    void signal();
    void wait();
    bool try_wait();

    semaphore(const semaphore&)             = delete;
    semaphore& operator=(const semaphore&)  = delete;

private:
    std::atomic_int m_counter;
};

// Empty binary semaphore, acting like spinlock of some kind that can have
// only two values - 0 or 1
class binary_semaphore
{
public:
    constexpr binary_semaphore()
        :m_flag{0} { }

    void signal();
    void wait();
    bool try_wait();

    binary_semaphore(const binary_semaphore&)             = delete;
    binary_semaphore& operator=(const binary_semaphore&)  = delete;

private:
    std::atomic_bool m_flag;
};

} // namespace ecl

#endif // LIB_THREAD_NO_OS_SEMAPHORE_
