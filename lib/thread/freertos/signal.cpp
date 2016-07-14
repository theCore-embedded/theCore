#include <ecl/thread/signal.hpp>

#include <FreeRTOS.h>
#include <task.h>

#include <common/irq.hpp>
#include <ecl/assert.h>

namespace ecl
{

namespace os
{

namespace signal
{

ecl::err send(const thread_handle &handle)
{
    if (ecl::irq::in_isr()) {
        // pxHigherPriorityTaskWoken argument is not interesting right now.
        vTaskNotifyGiveFromISR(handle, nullptr);
    } else {
        xTaskNotifyGive(handle);
    }

    return ecl::err::ok;
}

void clear()
{
    ecl_assert(!ecl::irq::in_isr());
    auto handle = this_thread::get_handle();
    // Ignore return value. Don't care whether notification was sent or not.
    // This might be changed in future.
    xTaskNotifyStateClear(handle);
}

void wait()
{
    ecl_assert(!ecl::irq::in_isr());
    constexpr BaseType_t clear_count = pdFALSE;
    // Ignore return value. Don't care about current notification counter.
    // This might be changed in future.
    ulTaskNotifyTake(clear_count, portMAX_DELAY);
}

ecl::err try_wait()
{
    ecl_assert(!ecl::irq::in_isr());
    constexpr uint32_t bits_to_clear_entry  = 0;
    constexpr uint32_t bits_to_clear_exit   = 0;
    constexpr TickType_t ticks_to_wait      = 0; // Return immediately
    // Ignore notification argument. Don't care about current notification counter.
    // This might be changed in future.
    auto rc = xTaskNotifyWait(bits_to_clear_entry,
                              bits_to_clear_exit,
                              nullptr,
                              ticks_to_wait);

    return rc == pdPASS ? ecl::err::ok : ecl::err::again;
}

} // namespace signal

} // namespace os

} // namespace ecl
