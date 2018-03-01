/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//!
//! \file
//! \brief Memory mapped I/O abstractions.
//! \details Taken from https://github.com/kensmith/cppmmio

#ifndef PLATFORM_COMMON_MMIO_
#define PLATFORM_COMMON_MMIO_

#include <limits>

namespace ecl
{

namespace mmio
{

//! Generates bitstring of consequent '1's of the given width.
//! \param[in] width Desired width. Must not be 0.
constexpr uint32_t generate_unshifted_mask(uint32_t width)
{
    return width == 1 ? 1 : generate_unshifted_mask(width - 1) | 1 << (width - 1);
}

//! Generates shifted mask.
//! \param[in] width  Width of the mask.
//! \param[in] offset Offset of the mask.
constexpr uint32_t generate_mask(uint32_t width, uint32_t offset)
{
    return generate_unshifted_mask(width) << offset;
}

//! Read-only mutability trait.
struct ro_mut
{
    //! Reads value from a memory-mapped register.
    //! \param[in] device Register address.
    //! \param[in] offset Offset of the data inside register.
    //! \param[in] mask   Mask to extract data from the register.
    static uint32_t read(volatile const uint32_t *device, uint32_t offset, uint32_t mask)
    {
        return (*device & mask) >> offset;
    }
};

//! Write-only mutability trait.
struct wo_mut
{
    //! Writes value to a memory-mapped register that doesn't support reading.
    //! \param[in] device Register address.
    //! \param[in] offset Offset of the data inside register.
    //! \param[in] mask   Mask to extract data from the register.
    //! \param[in] value  Value to write into the register.
    static void write(volatile uint32_t *device, uint32_t offset, uint32_t mask, uint32_t value)
    {
        *device = (value << offset) & mask;
    }
};

//! Read-write mutability trait.
struct rw_mut : ro_mut
{
    //! Writes value to a memory-mapped register that support reading.
    //! \param[in] device Register address.
    //! \param[in] offset Offset of the data inside register.
    //! \param[in] mask   Mask to extract data from the register.
    //! \param[in] value  Value to write into the register.
    static void write(volatile uint32_t *device, uint32_t offset, uint32_t mask, uint32_t value)
    {
        *device = (*device & ~mask) | ((value << offset) & mask);
    }
};

//! Memory-mapped I/O register.
//! \tparam Mut     Mutability trait.
//! \tparam addr    Address of a memory map register.
//! \tparam offset  Offset of the data inside register.
//! \tparam width   Width of a data stored in the register.
//! \sa rw_mut \sa ro_mut \sa wo_mut
template<typename Mut, std::uintptr_t addr, uint32_t offset, uint32_t width>
struct reg
{
    static_assert(width != 0, "zero width");
    static_assert(width + offset <= std::numeric_limits<uint32_t>::digits, "width overflow");

    //! Reads data from a register.
    static uint32_t read()
    {
        return Mut::read(reinterpret_cast<volatile uint32_t*>(addr), offset, generate_mask(width, offset));
    }

    //! Writes data to a register.
    //! \param[in] val Value to write.
    static void write(uint32_t val)
    {
        Mut::write(reinterpret_cast<volatile uint32_t*>(addr), offset, generate_mask(width, offset), val);
    }
};

} // namespace mmio

} // namespace ecl

#endif // PLATFORM_COMMON_MMIO_
