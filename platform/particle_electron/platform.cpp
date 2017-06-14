#include "application.h"
#include "stdarg.h"

//------------------------------------------------------------------------------
// System timer


//! User-overridable timer event handler
extern "C"
__attribute__((weak))
void systmr_handler()
{
    // Do nothing if not overriden
}

//! Struct-container for timer callback and corresponding object.
static struct tmr_cb
{
    //! Handles timeout events
    void on_timeout()
    {
        event_cnt++;
        systmr_handler();
    }

    //! Total timer events counter. May wrap.
    volatile uint32_t event_cnt = 0;
} cb_obj;

//! Timer speed in hertz.
static constexpr auto tmr_speed_hz = 20;

//! Particle timer object, used as system timer.
static Timer particle_tmr{1000 / tmr_speed_hz, &tmr_cb::on_timeout, cb_obj};

namespace ecl
{

namespace systmr
{

 //! Enables timer, start counting
void enable()
{
    particle_tmr.start();
}

//! Disables timer, stop counting
void disable()
{
    particle_tmr.stop();
}

uint32_t speed()
{
    return tmr_speed_hz;
}

uint32_t events()
{
    return cb_obj.event_cnt;
}

} // namespace systmr


} // namespace ecl

//------------------------------------------------------------------------------
// Serial-driver dispatcher

namespace ecl
{
//! External dispatch routine for serial drivers.
extern void serial_tx_dispatch();
}

//------------------------------------------------------------------------------
// Startup-related

SYSTEM_MODE(MANUAL);

// Main function will be launched in separate thread
static Thread main_thread;

// Main routine itself
extern int main();

// Provided to setup user-related periphery
extern "C" void board_init();


void main_thread_fn(void *param)
{
    (void)param;
    main();

    // Exit from main is a bad practice in the embedded app.
    abort();
}

//------------------------------------------------------------------------------

void setup()
{
    Serial.begin();

    // TODO: avoid it, possibly disable if in release mode
    delay(3000); // Hack! Give some time for USB debug connection to settle.

    main_thread = Thread{"main_thread", main_thread_fn};

    board_init();
}

void loop()
{
    // Process TX events if any.
    ecl::serial_tx_dispatch();
    // This may impose delays with triggering some event
    // processing in the Electron/Particle main loop
    delay(50);
}
