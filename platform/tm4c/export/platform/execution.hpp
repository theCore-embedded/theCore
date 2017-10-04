/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Execution control for TM4C platform.
#ifndef TM4C_EXECUTION_HPP_
#define TM4C_EXECUTION_HPP_

#include <stdbool.h>
#include <stdint.h>
#include <sysctl.h>
#include <systick.h>
#include <limits.h>
#include <interrupt.h>

#include <hw_nvic.h>
#include <hw_types.h>
#include <hw_memmap.h>

// ARM-CM architecture exports execution header that includes ARM CMSIS
// driver. This inclusion is explicitely commented to prevent any conflicts
// because ARM CMSIS is not compatible with TI driver library.
// #include <arch/execution.hpp>

//------------------------------------------------------------------------------
// Due to incompatibility of DriverLib with CMSIS (register names redefined)
// part of CMSIS library is included here.

/* IO definitions (access restrictions to peripheral registers) */
#ifdef __cplusplus
  #define   __I     volatile             /*!< defines 'read only' permissions                 */
#else
  #define   __I     volatile const       /*!< defines 'read only' permissions                 */
#endif
#define     __O     volatile             /*!< defines 'write only' permissions                */
#define     __IO    volatile             /*!< defines 'read / write' permissions              */

/* following defines should be used for structure members */
#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
#define     __OM     volatile            /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */

#define SCB_ICSR_VECTACTIVE_Pos             0                                             /*!< SCB ICSR: VECTACTIVE Position */
#define SCB_ICSR_VECTACTIVE_Msk            (0x1FFUL << SCB_ICSR_VECTACTIVE_Pos)           /*!< SCB ICSR: VECTACTIVE Mask */

//------------------------------------------------------------------------------

/**
  \brief  Structure type to access the Core Debug Register (CoreDebug).
 */
typedef struct
{
  __IOM uint32_t DHCSR;                  /*!< Offset: 0x000 (R/W)  Debug Halting Control and Status Register */
  __OM  uint32_t DCRSR;                  /*!< Offset: 0x004 ( /W)  Debug Core Register Selector Register */
  __IOM uint32_t DCRDR;                  /*!< Offset: 0x008 (R/W)  Debug Core Register Data Register */
  __IOM uint32_t DEMCR;                  /*!< Offset: 0x00C (R/W)  Debug Exception and Monitor Control Register */
} CoreDebug_Type;


/* Debug Exception and Monitor Control Register Definitions */
#define CoreDebug_DEMCR_TRCENA_Pos         24U                                            /*!< CoreDebug DEMCR: TRCENA Position */
#define CoreDebug_DEMCR_TRCENA_Msk         (1UL << CoreDebug_DEMCR_TRCENA_Pos)            /*!< CoreDebug DEMCR: TRCENA Mask */

//------------------------------------------------------------------------------

/**
  \brief  Structure type to access the Data Watchpoint and Trace Register (DWT).
 */
typedef struct
{
  __IOM uint32_t CTRL;                   /*!< Offset: 0x000 (R/W)  Control Register */
  __IOM uint32_t CYCCNT;                 /*!< Offset: 0x004 (R/W)  Cycle Count Register */
  __IOM uint32_t CPICNT;                 /*!< Offset: 0x008 (R/W)  CPI Count Register */
  __IOM uint32_t EXCCNT;                 /*!< Offset: 0x00C (R/W)  Exception Overhead Count Register */
  __IOM uint32_t SLEEPCNT;               /*!< Offset: 0x010 (R/W)  Sleep Count Register */
  __IOM uint32_t LSUCNT;                 /*!< Offset: 0x014 (R/W)  LSU Count Register */
  __IOM uint32_t FOLDCNT;                /*!< Offset: 0x018 (R/W)  Folded-instruction Count Register */
  __IM  uint32_t PCSR;                   /*!< Offset: 0x01C (R/ )  Program Counter Sample Register */
  __IOM uint32_t COMP0;                  /*!< Offset: 0x020 (R/W)  Comparator Register 0 */
  __IOM uint32_t MASK0;                  /*!< Offset: 0x024 (R/W)  Mask Register 0 */
  __IOM uint32_t FUNCTION0;              /*!< Offset: 0x028 (R/W)  Function Register 0 */
        uint32_t RESERVED0[1U];
  __IOM uint32_t COMP1;                  /*!< Offset: 0x030 (R/W)  Comparator Register 1 */
  __IOM uint32_t MASK1;                  /*!< Offset: 0x034 (R/W)  Mask Register 1 */
  __IOM uint32_t FUNCTION1;              /*!< Offset: 0x038 (R/W)  Function Register 1 */
        uint32_t RESERVED1[1U];
  __IOM uint32_t COMP2;                  /*!< Offset: 0x040 (R/W)  Comparator Register 2 */
  __IOM uint32_t MASK2;                  /*!< Offset: 0x044 (R/W)  Mask Register 2 */
  __IOM uint32_t FUNCTION2;              /*!< Offset: 0x048 (R/W)  Function Register 2 */
        uint32_t RESERVED2[1U];
  __IOM uint32_t COMP3;                  /*!< Offset: 0x050 (R/W)  Comparator Register 3 */
  __IOM uint32_t MASK3;                  /*!< Offset: 0x054 (R/W)  Mask Register 3 */
  __IOM uint32_t FUNCTION3;              /*!< Offset: 0x058 (R/W)  Function Register 3 */
} DWT_Type;

/* DWT Control Register Definitions */
#define DWT_CTRL_CYCCNTENA_Pos              0U                                         /*!< DWT CTRL: CYCCNTENA Position */
#define DWT_CTRL_CYCCNTENA_Msk             (0x1UL /*<< DWT_CTRL_CYCCNTENA_Pos*/)       /*!< DWT CTRL: CYCCNTENA Mask */

/** \brief  Structure type to access the System Control Block (SCB).
 */
typedef struct
{
  __I  uint32_t CPUID;                   /*!< Offset: 0x000 (R/ )  CPUID Base Register                                   */
  __IO uint32_t ICSR;                    /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register                  */
  __IO uint32_t VTOR;                    /*!< Offset: 0x008 (R/W)  Vector Table Offset Register                          */
  __IO uint32_t AIRCR;                   /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register      */
  __IO uint32_t SCR;                     /*!< Offset: 0x010 (R/W)  System Control Register                               */
  __IO uint32_t CCR;                     /*!< Offset: 0x014 (R/W)  Configuration Control Register                        */
  __IO uint8_t  SHP[12];                 /*!< Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
  __IO uint32_t SHCSR;                   /*!< Offset: 0x024 (R/W)  System Handler Control and State Register             */
  __IO uint32_t CFSR;                    /*!< Offset: 0x028 (R/W)  Configurable Fault Status Register                    */
  __IO uint32_t HFSR;                    /*!< Offset: 0x02C (R/W)  HardFault Status Register                             */
  __IO uint32_t DFSR;                    /*!< Offset: 0x030 (R/W)  Debug Fault Status Register                           */
  __IO uint32_t MMFAR;                   /*!< Offset: 0x034 (R/W)  MemManage Fault Address Register                      */
  __IO uint32_t BFAR;                    /*!< Offset: 0x038 (R/W)  BusFault Address Register                             */
  __IO uint32_t AFSR;                    /*!< Offset: 0x03C (R/W)  Auxiliary Fault Status Register                       */
  __I  uint32_t PFR[2];                  /*!< Offset: 0x040 (R/ )  Processor Feature Register                            */
  __I  uint32_t DFR;                     /*!< Offset: 0x048 (R/ )  Debug Feature Register                                */
  __I  uint32_t ADR;                     /*!< Offset: 0x04C (R/ )  Auxiliary Feature Register                            */
  __I  uint32_t MMFR[4];                 /*!< Offset: 0x050 (R/ )  Memory Model Feature Register                         */
  __I  uint32_t ISAR[5];                 /*!< Offset: 0x060 (R/ )  Instruction Set Attributes Register                   */
       uint32_t RESERVED0[5];
  __IO uint32_t CPACR;                   /*!< Offset: 0x088 (R/W)  Coprocessor Access Control Register                   */
} SCB_Type;

//------------------------------------------------------------------------------

#define SCS_BASE            (0xE000E000UL)                            /*!< System Control Space Base Address */
#define SCB_BASE            (SCS_BASE +  0x0D00UL)                    /*!< System Control Block Base Address */
#define SCB                 ((SCB_Type       *)     SCB_BASE      )   /*!< SCB configuration struct */
#define CoreDebug_BASE      (0xE000EDF0UL)                            /*!< Core Debug Base Address */
#define CoreDebug           ((CoreDebug_Type *)     CoreDebug_BASE)   /*!< Core Debug configuration struct */

//------------------------------------------------------------------------------

/* DWT Base Address already defined in driverlib
 * #define DWT_BASE            (0xE0001000UL)
 */
#define DWT                 ((DWT_Type       *)     DWT_BASE      )   /*!< DWT configuration struct */

//------------------------------------------------------------------------------

namespace ecl
{

//! \brief Aborts execution of currently running code. Never return.
__attribute__((noreturn))
static inline void abort()
{
    IntMasterDisable();
    for(;;);
}

//! Waits for interrupts.
//! \details Processor will stop executing until any interrupt will occur.
//! See also: http://infocenter.arm.com/help/topic/com.arm.doc.ihi0014q/CJAJGICJ.html
static inline void wfi()
{
    SysCtlSleep();
}

//! Waits for events.
//! \details Processor will stop executing until any event will occur.
//! \note Event flag will be set regardless of execution state. To track
//! spurious wakeup, additional flags must be checked.
//! See also: http://infocenter.arm.com/help/topic/com.arm.doc.ihi0014q/CJAJGICJ.html
static inline void wfe()
{
    asm volatile ("wfe");
}

#if USE_SYSTMR

namespace systmr
{

//! Represents the minimum and the maximun possible values for SYSTMR frequency.
//! This allows to guarantee the correct SYSTRM frequency regardless of SystemCoreClock.
static const uint32_t systmr_freq_min = 20;
static const uint32_t systmr_freq_max = 1000;

static_assert((THECORE_CONFIG_SYSTMR_FREQ >= systmr_freq_min) &&
               (THECORE_CONFIG_SYSTMR_FREQ <= systmr_freq_max),
               "The value of the THECORE_CONFIG_SYSTMR_FREQ frequency is out of the range.");

 //! Enables timer, start counting
static inline void enable()
{
    uint32_t reload_val = (1000 / THECORE_CONFIG_SYSTMR_FREQ) * SysCtlClockGet() / 1000;

    SysTickPeriodSet(reload_val);

    // reset SysTick counter
    HWREG(NVIC_ST_CURRENT) = 0;

    SysTickIntEnable();
    SysTickEnable();
}

//! Disables timer, stop counting
static inline void disable()
{
    SysTickDisable();
    SysTickIntDisable();
}

//! Gets speed in which timer generate events.
//! \details _Each event generates interrupt_, but not necessarily timer interrupt
//! is passed to the user. In other words, each event will wake-up processor.
//! Useful in conjunction with WFI.
//! \retval System timer events generation speed in Hz.
static inline auto speed()
{
    uint32_t reload_val = (1000 / THECORE_CONFIG_SYSTMR_FREQ) * SysCtlClockGet() / 1000;

    return SysCtlClockGet() / reload_val;
}

//! Gets amount of events occurred so far.
//! \details Events counter may not reset after disable() call.
//! \retval Amount of events occured so far
uint32_t events();

} // namespace systmr

#endif // USE_SYSTMR

//! \brief Performs a dummy busy wait for specified amount of milliseconds.
//! \param ms number of milliseconds to wait.
//!
//! This function is useful for a short delays.
//!
//! \return None.
static inline void spin_wait(uint32_t ms)
{
    const short SysCtlDelay_ticks = 3;

    // handle overflow
    uint64_t ticks_left = (SysCtlClockGet() / 1000L / SysCtlDelay_ticks) * ms;
    while (UINT_MAX < ticks_left) {
        SysCtlDelay(UINT_MAX);
        ticks_left -= UINT_MAX;
    }

    SysCtlDelay(ticks_left);
}

} // namespace ecl

#endif  // TM4C_EXECUTION_HPP_
