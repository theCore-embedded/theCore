#ifndef LIB_UTILS_ASSERT_HPP_
#define LIB_UTILS_ASSERT_HPP_

#if __STDC_HOSTED__
#include <assert.h>
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NDEBUG // No-debug switch

#if __STDC_HOSTED__
// Assert itself
#define ecl_assert(cond) \
    assert(cond)

// Prints some useful message before asserting
#define ecl_assert_msg(cond, message) \
    do { if (!(cond)) { puts(message); assert(cond); } } while (0)

#else // __STDC_HOSTED__

// These two is same as above but just works without libc implementation

#define ecl_assert(COND) \
do { \
    if (!(COND)) { \
        ecl_assert_failed((#COND), NULL, __FILE__, __func__, __LINE__);\
    } \
} while(0)

#define ecl_assert_msg(COND, MESSAGE) \
    do { \
        if (!(COND)) { \
            ecl_assert_failed((#COND), (MESSAGE), __FILE__, __func__, __LINE__);\
        } \
    } while(0)

// Real assert routine
void ecl_assert_failed(const char *assertion,
                       const char *message,
                       const char *file,
                       const char *func,
                       unsigned int line);

#endif // __STDC_HOSTED__

#else // NDEBUG

#define ecl_assert(cond) ((void)(cond))
#define ecl_assert_msg(cond, message) ((void)(cond))

#endif // NDEBUG

#ifdef __cplusplus
}
#endif

#endif // LIB_UTILS_ASSERT_HPP_
