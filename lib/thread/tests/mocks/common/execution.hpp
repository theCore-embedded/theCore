#ifndef COMMON_EXECUTION_MOCK_HPP_
#define COMMON_EXECUTION_MOCK_HPP_

#include <thread>

namespace ecl
{

static inline void wait_for(uint32_t ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

} // namespace ecl

#endif // COMMON_EXECUTION_MOCK_HPP_
