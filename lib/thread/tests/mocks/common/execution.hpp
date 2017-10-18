/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef COMMON_EXECUTION_MOCK_HPP_
#define COMMON_EXECUTION_MOCK_HPP_

#include <thread>

namespace ecl
{

template<class Predicate>
static inline bool wait_for(uint32_t ms, Predicate pred)
{
    const uint32_t quant = 20;

    do {
        if (pred()) {
            return true;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(quant));
    } while (static_cast<int>(ms -= 20) > 0);

    return false;
}

} // namespace ecl

#endif // COMMON_EXECUTION_MOCK_HPP_
