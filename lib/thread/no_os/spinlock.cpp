/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ecl/thread/spinlock.hpp>
#include <platform/execution.hpp>

constexpr ecl::spinlock::spinlock()
    :m_flag ATOMIC_FLAG_INIT // Braces '{}' are included in macro body of ATOMIC_FLAG_INIT
{
}

void ecl::spinlock::lock()
{
    while (m_flag.test_and_set(std::memory_order_acquire)) {
#ifdef USE_WFI_WFE
        ecl::wfe();
#endif
    }
}

void ecl::spinlock::unlock()
{
    m_flag.clear(std::memory_order_release);
}
