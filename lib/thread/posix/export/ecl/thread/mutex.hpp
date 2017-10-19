/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LIB_THREAD_HOST_MUTEX_
#define LIB_THREAD_HOST_MUTEX_

#include <mutex>

namespace ecl
{

class mutex
{
public:
    mutex();
    ~mutex();

    void lock();
    void unlock();
    bool try_lock();

    mutex(const mutex&)             = delete;
    mutex& operator=(const mutex&)  = delete;
private:
    std::mutex m_mutex;
};

}


#endif // LIB_THREAD_HOST_MUTEX_
