/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COMMON_EXECUTION_MOCK_HPP_
#define COMMON_EXECUTION_MOCK_HPP_

#include <thread>

namespace ecl
{

static inline void wait_for(uint32_t ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

} // namespace ecl

#endif // COMMON_EXECUTION_MOCK_HPP_
