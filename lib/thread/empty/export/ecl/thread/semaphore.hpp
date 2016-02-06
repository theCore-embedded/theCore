#ifndef LIB_THREAD_EMPTY_SEMAPHORE_
#define LIB_THREAD_EMPTY_SEMAPHORE_

#include <atomic>

namespace ecl
{

// Empty semaphore, acting like spinlock of some kind
class semaphore
{
public:
    constexpr semaphore();

    void signal();
    void wait();

private:
    std::atomic_int m_counter;
};

}

#endif // LIB_THREAD_EMPTY_SEMAPHORE_
