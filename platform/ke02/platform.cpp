extern "C" {
#include <sysinit.h>
}

#include <common.h>
#include <wdog.h>


extern "C"
void platform_init()
{

}

extern "C"
void SystemInit()
{
    /* Disable the watchdog timer */
    WDOG_DisableWDOGEnableUpdate();

    sysinit();
}
