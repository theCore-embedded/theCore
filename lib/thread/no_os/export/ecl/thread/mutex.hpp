/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Mutex implementation for projects without OS support.

#ifndef LIB_THREAD_NO_OS_MUTEX_
#define LIB_THREAD_NO_OS_MUTEX_

namespace ecl
{

class mutex
{
public:
    mutex() = default;

    void lock();
    void unlock();
    bool try_lock();

    mutex(const mutex&)             = delete;
    mutex& operator=(const mutex&)  = delete;
};

} // namespace ecl

#endif // LIB_THREAD_NO_OS_MUTEX_
