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

    // Systimer implementation allows to use more robust and convenient
    // version of wait_for.
    // TODO: Drop conditional compilation when #247 will be ready.
#ifdef USE_SYSTMR
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
#else
    // Some platform support not yet include RTC or any other time source.
    // Time reference is possibly provided by simple clock counting, which
    // can be wrapped.
    // Also, wait_for implementation without systimer is pretty impractical,
    // so its usage is avoided here.
    auto now = std::chrono::milliseconds(0);

    int cnt;

    while (((cnt = m_counter.load()) <= 0   // Wait till counter goes up enough
                                            // to avoid block.
               // After counter uprised, try to set new value.
               // Magic here is that after counter is checked new thread can
               // decrement it. Compare-and-exchange makes sure that counter
               // is not modified by other threads.
               || !(exch = m_counter.compare_exchange_weak(cnt, cnt - 1)))
           // Keep track of timeout.
           && now++ < ms)
    {
        ecl::wait_for(1);
    }
#endif // USE_SYSTMR

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
    // TODO: Drop conditional compilation when #247 will be ready.
#ifdef USE_SYSTMR
    wait_for(ms.count(), [&ex, this] {
        return ex = m_flag.exchange(false);
    });
#else

    // Some platform support not yet include RTC.
    // Time reference is possibly provided by simple clock counting, which
    // can be wrapped.
    // Also, wait_for implementation without systimer is pretty impractical,
    // so its usage is avoided here.
    auto now = std::chrono::milliseconds(0);

    while (!(ex = m_flag.exchange(false)) && now++ < ms) {
        ecl::wait_for(1);
    }
#endif // USE_SYSTMR

    // If last exchanged value is false, it means that timeout was reached.
    return ex;
}
