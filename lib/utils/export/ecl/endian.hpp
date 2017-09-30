/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LIB_ECL_ENDIAN_HPP_
#define LIB_ECL_ENDIAN_HPP_

#include <type_traits>

namespace ecl {

#if __BYTE_ORDER__ ==__ORDER_LITTLE_ENDIAN__

// No matter which conversion is required (from host to LE, or from LE to host)
// this routine will not change the byte order. Since host works with
// Little Endian order.
template< typename Integer >
Integer LE(Integer in)
{
    static_assert(std::is_integral< Integer >::value, "Input must be integer");
    return in;
}

// Set of routines, converting from\to Big Endian order

static inline uint16_t BE(uint16_t in)
{
    return __builtin_bswap16(in);
}

static inline int16_t BE(int16_t in)
{
    return __builtin_bswap16(in);
}

static inline uint32_t BE(uint32_t in)
{
    return __builtin_bswap32(in);
}

static inline int32_t BE(int32_t in)
{
    return __builtin_bswap32(in);
}

static inline uint64_t BE(uint64_t in)
{
    return __builtin_bswap64(in);
}

static inline int64_t BE(int64_t in)
{
    return __builtin_bswap64(in);
}

#else
#error "Not supported endianness! Implementation required"
#endif

}

#endif
