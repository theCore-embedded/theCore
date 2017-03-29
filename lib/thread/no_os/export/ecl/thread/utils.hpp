//! \file
//! \brief Utilities for projects without OS support.
//!
#ifndef LIB_THREAD_NO_OS_UTILS_HPP_
#define LIB_THREAD_NO_OS_UTILS_HPP_

#include <common/execution.hpp>

namespace ecl
{

namespace this_thread
{

//!
//! \brief Put task at sleep for given amount of milliseconds.
//! \param[in] ms Milliseconds to sleep.
//!
static inline void sleep_for(uint32_t ms)
{
    ecl::spin_wait(ms);
}

} // namespace this_thread

} // namespace ecl

#endif // LIB_THREAD_NO_OS_UTILS_HPP_
