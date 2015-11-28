#ifndef LIB_ECL_ASSERT_HPP_
#define LIB_ECL_ASSERT_HPP_

#include "iostream.hpp"

#define assert(cond) \
    do { \
        if (!(cond)) { \
            ecl::cout << __FILENAME__ << ':' << __LINE__ << " " << __func__ << \
                      ": assertion failed: " << #cond << ecl::endl; \
        } \
    } while (0)

#endif // LIB_ECL_ASSERT_HPP_
