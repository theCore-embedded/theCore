//! \file
//! \brief Various routines for particle electron platform, altering execution flow.

#ifndef PLATFORM_EXECUTION_HPP_
#define PLATFORM_EXECUTION_HPP_

#include <stdint.h>

namespace ecl
{

//! \brief Gets core clock speed.
//! \return Current clock speed.
static inline uint32_t clk_spd()
{
    // TODO
    return 1;
}

//! \brief Gets current clock count.
//! \details Uses DWT register.
//! See http://www.carminenoviello.com/2015/09/04/precisely-measure-microseconds-stm32/
//! \return Current clock count.
static inline uint32_t clk()
{
    // TODO
    return 1;
}

//! \brief Aborts execution of currently running code. Never return.
__attribute__((noreturn))
static inline void abort()
{
    // TODO
    for(;;);
}

} // namespace ecl

#endif // PLATFORM_EXECUTION_HPP_
