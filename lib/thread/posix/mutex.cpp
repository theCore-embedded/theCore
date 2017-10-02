/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ecl/thread/mutex.hpp>

ecl::mutex::mutex()
    :m_mutex{}
{
}

ecl::mutex::~mutex()
{
}


void ecl::mutex::lock()
{
    m_mutex.lock();
}

void ecl::mutex::unlock()
{
    m_mutex.unlock();
}

bool ecl::mutex::try_lock()
{
    return m_mutex.try_lock();
}
