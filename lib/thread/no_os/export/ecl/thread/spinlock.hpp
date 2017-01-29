//! \file
//! \brief Simple spinlock. Without any OS support.
//!
#ifndef LIB_THREAD_NO_OS_SPINLOCK_
#define LIB_THREAD_NO_OS_SPINLOCK_

#include <atomic>

namespace ecl
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

} // namespace ecl

#endif // LIB_THREAD_NO_OS_SPINLOCK_
