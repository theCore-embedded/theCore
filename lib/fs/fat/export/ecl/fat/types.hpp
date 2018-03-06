/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief FATFS convinient aliases

#ifndef FATFS_TYPES_HPP_
#define FATFS_TYPES_HPP_

#include <ecl/pool.hpp>

namespace ecl
{

namespace fat
{

//! \addtogroup lib Libraries and utilities
//! @{

//! \defgroup fs Filesystem support
//! @{

//! \defgroup fat FAT filesystem
//! @{

// Defines common allocator type for all fat objects
using allocator = ecl::pool_allocator< uint8_t >;

//! @}

//! @}

//! @}

} // namespace fat

} // namespace ecl

#endif // FATFS_TYPES_HPP_
