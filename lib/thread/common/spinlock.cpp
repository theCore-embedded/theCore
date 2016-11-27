#include <ecl/thread/common/spinlock.hpp>


constexpr ecl::common::spinlock::spinlock()
    :m_flag ATOMIC_FLAG_INIT // Braces '{}' are included in macro body of ATOMIC_FLAG_INIT
{
}

void ecl::common::spinlock::lock()
{
    while (m_flag.test_and_set(std::memory_order_acquire))
        ;
}

void ecl::common::spinlock::unlock()
{
    m_flag.clear(std::memory_order_release);
}

