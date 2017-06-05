#include "application.h"
#include "stdarg.h"

SYSTEM_MODE(MANUAL);

// Main function will be launched in separate thread
static Thread main_thread;

// Main routine itself
extern int main();

// Provided to setup user-related periphery
extern "C" void board_init();

namespace ecl
{
//! External dispatch routine for serial drivers.
extern void serial_tx_dispatch();
}

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
