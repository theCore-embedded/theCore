/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BUS_MOCKS_MUTEX_HPP_
#define BUS_MOCKS_MUTEX_HPP_

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <ecl/err.hpp>

#include <ecl/thread/mutex.hpp>

namespace ecl
{

void mutex::lock()
{
    mock("mutex").actualCall("lock");
}

void mutex::unlock()
{
    mock("mutex").actualCall("unlock");
}

bool mutex::try_lock()
{
    return mock("mutex").actualCall("try_lock").returnIntValue();
}

} // namespace ecl


#endif // BUS_MOCKS_MUTEX_HPP_
