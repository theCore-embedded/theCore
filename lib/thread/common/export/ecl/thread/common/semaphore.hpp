#ifndef LIB_THREAD_COMMON_SEMAPHORE_
#define LIB_THREAD_COMMON_SEMAPHORE_

#include <ecl/err.hpp>

#include <atomic>

namespace ecl
{

namespace common
{

// Empty semaphore, acting like spinlock of some kind
class semaphore
{
public:
    constexpr semaphore()
        :m_counter{0} { }

    void signal();
    void wait();
    ecl::err try_wait();

    semaphore(const semaphore&)             = delete;
    semaphore& operator=(const semaphore&)  = delete;

private:
    std::atomic_int m_counter;
};

}

}

#endif // LIB_THREAD_COMMON_SEMAPHORE_
