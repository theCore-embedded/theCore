//! \file
//! \brief User-side code that should be compiled directly into resulting
//! application. Moved to separate file and advertised via interface CMake
//! library to avoid linking issues.

#include "application.h"
#include "stdarg.h"

// TODO: make it configurable
SYSTEM_MODE(MANUAL);

// Main function will be launched in separate thread
static Thread main_thread;

// Main routine itself
extern int main();

// Provided to setup user-related periphery
extern "C" void board_init();

//! Thread main routine, proxy to the main itself.
void main_thread_fn(void *param)
{
    (void)param;
    main();

    // Exit from main is a bad practice in the embedded app.
    abort();
}

//------------------------------------------------------------------------------
// Device-driver dispatchers. Internal interface.

namespace ecl
{

//! Container for \sa dispatcher_timer::check_xfer()
static struct dispatcher_timer
{
    //! Runs particular dispatcher for every platform device driver.
    void check_xfer()
    {
        extern void serial_tx_dispatch();
        extern void i2c_dispatch();

        // According to RM, serial RX events are emitted by particle SDK
        // in the main event loop. So only TX is required.
        serial_tx_dispatch();
        i2c_dispatch();

        // Change period back, so that drivers will not be polled to often.
        m_tmr->changePeriod(1000);
    }

    Timer *m_tmr = nullptr;
} dispatcher_obj;

//! Soft-IRQ dispatcher timer.
static Timer dev_tmr(1000, &dispatcher_timer::check_xfer, dispatcher_obj);

//! Notifies main loop about pending transactions.
//! \detail Unfortunately, particle firmware does not provide access to control,
//! or at least get interrupts notifications (except for some slave-side cases
//! in I2C and SPI).
//! It is something that differs from theCore event model,
//! where everything runs asynchronously.
//! To compensate missing interrupts, they should be emulated by software.
//! By calling this routine should be the platform-level driver can receive
//! async event after xfer completetion.
//! Emulated IRQ executed by \sa dispatcher_timer
void notify_xfer_start()
{
    noInterrupts();
    // Force timer to be expired as soon as possible.
    dev_tmr.changePeriod(1);
    interrupts();
}

} // namespace ecl

//------------------------------------------------------------------------------

void setup()
{
    Serial.begin();

    // TODO: avoid it, possibly disable if in release mode
    delay(3000); // Hack! Give some time for USB debug connection to settle.

    ecl::dispatcher_obj.m_tmr = &ecl::dev_tmr;
    ecl::dev_tmr.start();

    // Run main function in separate thread to avoid this loop() nonsense.
    main_thread = Thread{"main_thread", main_thread_fn};

    board_init();
}

//! Loop with delay small enough to
void loop()
{
    delay(100);
}
