#ifndef FATFS_TYPES_HPP_
#define FATFS_TYPES_HPP_

#include <ecl/pool.hpp>

namespace fat
{
    // Defines common allocator type for all fat objects
    using allocator = ecl::pool_allocator< uint8_t >;
}

#endif
