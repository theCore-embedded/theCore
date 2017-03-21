#include <ecl/thread/spinlock.hpp>


constexpr ecl::spinlock::spinlock()
    :m_flag ATOMIC_FLAG_INIT // Braces '{}' are included in macro body of ATOMIC_FLAG_INIT
{
}

void ecl::spinlock::lock()
{
    while (m_flag.test_and_set(std::memory_order_acquire)) {
#ifdef USE_WFI_WFE
        ecl_wfi();
#endif
    }
}

void ecl::spinlock::unlock()
{
    m_flag.clear(std::memory_order_release);
}
