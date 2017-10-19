/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Implementation of the semaphore for targets without OS support.

#include <ecl/thread/semaphore.hpp>
#include <common/execution.hpp>

void ecl::semaphore::signal()
{
    m_counter++;
}

void ecl::semaphore::wait()
{
    int cnt;

    if ((cnt = m_counter.fetch_sub(1)) <= 0) {
        while (m_counter.load() < cnt) {
#ifdef USE_WFI_WFE
            ecl::wfe();
#endif
        }
    }
}

bool ecl::semaphore::try_wait(std::chrono::milliseconds ms)
{
    bool exch = false;

    wait_for(ms.count(), [&exch, this] {
        int cnt;

        return ((cnt = m_counter.load()) > 0 // Make sure counter is high
                                             // enough to avoid block.
                    // If counter is high enough, try to set new value.
                    // Magic here is that after counter is checked new thread can
                    // decrement it. Compare-and-exchange makes sure that counter
                    // is not modified by other threads.
                    && (exch = m_counter.compare_exchange_weak(cnt, cnt - 1)));
    });

    return exch;
}

//------------------------------------------------------------------------------

void ecl::binary_semaphore::signal()
{
    m_flag = true;
}

void ecl::binary_semaphore::wait()
{
    while (!m_flag) {
#ifdef USE_WFI_WFE
        ecl::wfe();
#endif
    }

    m_flag = false; // Semaphore taken.
}

bool ecl::binary_semaphore::try_wait(std::chrono::milliseconds ms)
{
    bool ex;

    // Systimer implementation allows to use more robust and convenient
    // version of wait_for.
    wait_for(ms.count(), [&ex, this] {
        return ex = m_flag.exchange(false);
    });

    // If last exchanged value is false, it means that timeout was reached.
    return ex;
}
