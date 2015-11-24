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

uint16_t BE(uint16_t in)
{
    return __builtin_bswap16(in);
}

int16_t BE(int16_t in)
{
    return __builtin_bswap16(in);
}

uint32_t BE(uint32_t in)
{
    return __builtin_bswap32(in);
}

int32_t BE(int32_t in)
{
    return __builtin_bswap32(in);
}

uint64_t BE(uint64_t in)
{
    return __builtin_bswap64(in);
}

int64_t BE(int64_t in)
{
    return __builtin_bswap64(in);
}

#else
#error "Not supported endianness! Implementation required"
#endif

}

#endif
