#ifndef COMMON_EXECUTION_MOCK_H_
#define COMMON_EXECUTION_MOCK_H_

#include <thread>

static inline void ecl_spin_wait(uint32_t ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

#endif // COMMON_EXECUTION_MOCK_H_
