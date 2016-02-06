#ifndef LIB_THREAD_COMMON_SPINLOCK_
#define LIB_THREAD_COMMON_SPINLOCK_

#include <atomic>

namespace ecl
{

// TODO: leave comment about why additional namespace is used
namespace common
{

class spinlock
{
public:
    constexpr spinlock();

    void lock();
    void unlock();

private:
    std::atomic_flag m_flag;
};

}

}


#endif // LIB_THREAD_COMMON_SPINLOCK_
