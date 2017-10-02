/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FATFS_TYPES_HPP_
#define FATFS_TYPES_HPP_

#include <ecl/pool.hpp>

namespace fat
{
    // Defines common allocator type for all fat objects
    using allocator = ecl::pool_allocator< uint8_t >;
}

#endif
