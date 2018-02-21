/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
    constexpr spinlock()
        :m_flag ATOMIC_FLAG_INIT // Braces '{}' are included in macro body of ATOMIC_FLAG_INIT
    { }

    void lock();
    void unlock();

private:
    std::atomic_flag m_flag;
};

} // namespace ecl

#endif // LIB_THREAD_NO_OS_SPINLOCK_
