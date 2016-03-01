#ifndef LIB_THREAD_FREERTOS_OS_UTILS_
#define LIB_THREAD_FREERTOS_OS_UTILS_

#include <cstdint>
#include <cstdint>

namespace os
{

//! OS-specific handle.
//! Yet we know that FreeRTOS uses void * as a typedef for TaskHandle_t
//! this will stay here.
using thread_handle = void *;

namespace this_thread
{

//!
//! \brief Yields current thread.
//! As for FreeRTOS whether current task will be rescheduled or not
//! depends on FreeRTOS configuration.
//!
void yield();

//!
//! \brief Put task at sleep for given amount of milliseconds.
//! \param[in] msesc Milliseconds to sleep.
//!
void sleep_for(uint32_t msesc);

//!
//! \brief Gets a thread handle of currently running thread.
//! \return Current thread handle
//!
thread_handle get_handle();

}

}

#endif
