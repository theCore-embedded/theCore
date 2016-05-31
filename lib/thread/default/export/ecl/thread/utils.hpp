//! \file
//! \brief Default utilities.
#ifndef LIB_THREAD_DEFAULT_UTILS_HPP_
#define LIB_THREAD_DEFAULT_UTILS_HPP_

#include <platform/execution.hpp>

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
    spin_wait(ms);
}

}

}

#endif // LIB_THREAD_DEFAULT_UTILS_HPP_
