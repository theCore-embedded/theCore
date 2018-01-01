/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LIB_UTILS_ASSERT_HPP_
#define LIB_UTILS_ASSERT_HPP_

#include <string.h>

// Some platforms have __STDC_HOSTED__ set to 1, while still lacks some
// libnano definitions for asserts
#if __STDC_HOSTED__ && THECORE_NATIVE_ASSERT
#include <assert.h>
#include <stdio.h>
#endif

#include <stddef.h>

#ifdef __cplusplus
#include <ecl/utils.hpp>

template<int N>
constexpr auto strlen_constexpr(const char (&str)[N])
{
    size_t len = 0;
    while (str[len++]) { }
    return len - 1;
}

//! Calculates offset of filename substring in given path string
template<int N>
constexpr auto fl_offset(const char (&path)[N])
{
    const char *fl = path;

    // Seek to end
    while (*fl) { fl++; }

    // Find last '/'
    while (fl != path && *fl != '/') { fl--; }

    // Skip leading '/', if present
    if (*fl == '/') { fl++; }

    return fl - path;
}

#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NDEBUG // No-debug switch

#if __STDC_HOSTED__ && THECORE_NATIVE_ASSERT
// Assert itself
#define ecl_assert(cond) \
    assert(cond)

// Prints some useful message before asserting
#define ecl_assert_msg(cond, message) \
    do { if (!(cond)) { puts(message); assert(cond); } } while (0)

#else // __STDC_HOSTED__ && THECORE_NATIVE_ASSERT

// These two is same as above but just works without libc implementation

#ifndef __cplusplus
#define ecl_assert_msg(COND, MESSAGE) \
    do { \
        if (!(COND)) { \
            ecl_assert_failed((#COND), (MESSAGE), __FILE__, __func__, __LINE__);\
        } \
    } while(0)
#else // __cplusplus

// C++ - optimized macro that shortens file paths
#define ecl_assert_msg(COND, MESSAGE) \
    do { \
        if (!(COND)) { \
            constexpr auto max_len = strlen_constexpr(__FILE__) + 1; \
            constexpr auto offset = fl_offset(__FILE__); \
            constexpr auto copy_len = max_len - offset; \
            struct path \
            { \
                constexpr static char const* str() { return __FILE__; } \
            }; \
            using flname = ecl::explode_chunk<path, offset, copy_len>; \
            ecl_assert_failed((#COND), (MESSAGE), flname::fl, __func__, __LINE__); \
        } \
    } while(0)
#endif

#define ecl_assert(COND) \
    ecl_assert_msg(COND, NULL)

// Real assert routine
void ecl_assert_failed(const char *assertion,
                       const char *message,
                       const char *file,
                       const char *func,
                       unsigned int line);

#endif // __STDC_HOSTED__ && THECORE_NATIVE_ASSERT

#else // NDEBUG

#define ecl_assert(cond) ((void)(cond))
#define ecl_assert_msg(cond, message) ((void)(cond))

#endif // NDEBUG

#ifdef __cplusplus
}
#endif

#endif // LIB_UTILS_ASSERT_HPP_
