extern "C" {
#include <sysinit.h>
}

#include <common.h>
#include <wdog.h>
#include "common.h"
#include "sysinit.h"
#include "sim.h"
#include "uart.h"
#include "ics.h"

#include <stdio.h>
#include <platform/console.hpp>

uint32_t SystemCoreClock = 40000000L;

extern "C"
uint8_t __atomic_exchange_1(volatile void *dest, uint8_t val, int model)
{
   (void)model;

   __disable_irq();

   uint8_t *u8_dest = (uint8_t*)dest;
   uint8_t old_val = *u8_dest;
   *u8_dest = val;

   __enable_irq();

   return old_val;
}

#ifdef THECORE_CONFIG_USE_CONSOLE

namespace ecl
{
    extern void bypass_console_init();
} // namespace ecl

#endif // THECORE_CONFIG_USE_CONSOLE

extern "C"
void platform_init()
{

}

extern "C"
void out_char(char c)
{
    ecl::bypass_putc(c);
}

extern "C"
void SystemInit()
{
    SIM_ConfigType  sSIMConfig;
    ICS_ConfigType  sICSConfig;

    ICS_Trim(ICS_TRIM_VALUE); /*!< trim IRC to 39.0625KHz and FLL output=40MHz */

    /*
     * Enable SWD pin, RESET pin
     */
    /*
     * NOTE: please make sure other register bits are also write-once and
     * need add other bit mask here if needed.
     */
#if defined(SPI0_PINREMAP)
    sSIMConfig.u32PinSel |= SIM_PINSEL_SPI0PS_MASK;
#endif

#if defined(OUTPUT_BUSCLK)
    sSIMConfig.sBits.bEnableCLKOUT = 1;      /* output bus clock if enabled */
#endif

#if defined(DISABLE_NMI)
    sSIMConfig.sBits.bDisableNMI = 1;
#endif

    #if !defined(CPU_KE04)
    /* make sure clocks to peripheral modules are enabled */
    sSIMConfig.u32SCGC |= SIM_SCGC_SWD_MASK | SIM_SCGC_FLASH_MASK |
                         SIM_SCGC_UART0_MASK | SIM_SCGC_UART1_MASK |
                         SIM_SCGC_UART2_MASK
                         ;
    #else
    sSIMConfig.u32SCGC |= SIM_SCGC_SWD_MASK | SIM_SCGC_FLASH_MASK |
                         SIM_SCGC_UART0_MASK
                         ;
    #endif

    #if !defined(CPU_KE02)
    /* bus clock divided by 2 */
    sSIMConfig.u32CLKDIV |= SIM_CLKDIV_OUTDIV2_MASK;
    #endif

    SIM_Init(&sSIMConfig);                   /* initialize SIM */

    sICSConfig.u8ClkMode = ICS_CLK_MODE_FEI;
    sICSConfig.bLPEnable = 0;
    sICSConfig.u32ClkFreq = 32;
    sICSConfig.oscConfig.bEnable = 0; /* disable OSC */

    ICS_Init(&sICSConfig);   /* initialize ICS */

    /* Disable the watchdog timer */
    WDOG_DisableWDOGEnableUpdate();

    ecl::bypass_console_init();

    void print_sys_log(void);
    print_sys_log();
}
