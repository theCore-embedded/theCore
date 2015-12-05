#ifndef LIB_ECL_ASSERT_HPP_
#define LIB_ECL_ASSERT_HPP_

#include "iostream.hpp"

// TODO: control verbosity of assertion from outside

#ifdef NDEBUG
#define assert(cond) ((void)0)
#else
#define assert(cond) \
    do { \
        if (!(cond)) { \
            ecl::cout << __FILE__ << ':' << __LINE__ << ": " << __func__  \
                      << " : assertion failed: " << (#cond) << ecl::endl; \
            for(;;); \
        } \
    } while(0)
#endif

#endif // LIB_ECL_ASSERT_HPP_
