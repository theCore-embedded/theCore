// Empty semaphore

#include <ecl/thread/semaphore.hpp>

constexpr ecl::semaphore::semaphore()
    :m_counter{0}
{ }

void ecl::semaphore::signal()
{
}

void ecl::semaphore::wait()
{
    auto old = m_counter.fetch_sub(1);
    while (old > m_counter.load()) { }
}
