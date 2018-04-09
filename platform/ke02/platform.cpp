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

#if defined(TRIM_IRC)
    /* if not trimmed, do trim first */
    ICS_Trim(ICS_TRIM_VALUE);
#endif
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

#if defined(XOSC_STOP_ENABLE)
    sICSConfig.oscConfig.bStopEnable = 1;    /*  enabled in stop mode */
#endif

#if defined(CRYST_HIGH_GAIN)
    sICSConfig.oscConfig.bGain = 1;           /* high gain */
#endif


#if  (EXT_CLK_FREQ_KHZ >=4000)
    sICSConfig.oscConfig.bRange = 1;           /* high range */
#endif

    sICSConfig.oscConfig.bEnable = 1;          /* enable OSC */
    sICSConfig.u32ClkFreq = EXT_CLK_FREQ_KHZ;

#if     defined(USE_FEE)
    sICSConfig.u8ClkMode = ICS_CLK_MODE_FEE;
#elif	defined(USE_FBE_OSC)
    sICSConfig.u8ClkMode = ICS_CLK_MODE_FBE_OSC;
#elif	defined(USE_FEE_OSC)
    sICSConfig.u8ClkMode = ICS_CLK_MODE_FEE_OSC;
#endif

    ICS_Init(&sICSConfig);   /* initialize ICS */

    /* Disable the watchdog timer */
    WDOG_DisableWDOGEnableUpdate();

    ecl::bypass_console_init();

    void print_sys_log(void);
    print_sys_log();
}
