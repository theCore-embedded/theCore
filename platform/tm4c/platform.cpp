#include <cstdint>
#include <sysctl.h>

#include <common/irq.hpp>

#ifdef CONFIG_BYPASS_CONSOLE_ENABLED
namespace ecl
{
extern void bypass_console_init();
} // namespace ecl
#endif // CONFIG_USE_CONSOLE

// Required by ARM ARCH startup code
extern "C" void SystemInit()
{
    auto cfg = SYSCTL_USE_OSC | // System clock is the osc clock (not PLL)
               SYSCTL_OSC_INT | // Osc clock is internal oscillator (PIOSC)
               SYSCTL_MAIN_OSC_DIS  | // Disable main oscillator
               SYSCTL_SYSDIV_2; // Processor clock is OSC / 2

    SysCtlClockSet(cfg);
}

extern "C" void platform_init()
{
    // TODO: implement

    // IRQ must be ready before anything else will start work
    ecl::irq::init_storage();

#ifdef CONFIG_BYPASS_CONSOLE_ENABLED
    ecl::bypass_console_init();
#endif
}
