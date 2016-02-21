#ifndef LIB_THREAD_FREERTOS_OS_UTILS_
#define LIB_THREAD_FREERTOS_OS_UTILS_

#include <cstdint>
#include <cstdint>

namespace os
{

namespace this_thread
{

void yield();
void sleep_for(uint32_t msesc);

}

}

#endif
