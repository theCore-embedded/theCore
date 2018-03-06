/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FATFS_FILESYSTEM_HPP_
#define FATFS_FILESYSTEM_HPP_

#include "ecl/fat/types.hpp"
#include "ecl/fat/file_inode.hpp"
#include "ecl/fat/dir_inode.hpp"
#include "src/pff.h"

#include <ecl/pool.hpp>
#include <ecl/fs/inode.hpp>

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

//! Petite FAT implementation over block device interface.
//! \param Block Block device class.
template<class Block>
class petit // TODO: rename it to 'petite_fat'
{
public:
    //! Mounts a system and returns the root inode.
    //! \return FATFS root inode.
    static fs::inode_ptr mount();

private:
    //! Gets an estimated single allocation size for inodes.
    static constexpr size_t get_alloc_blk_size();

    // Block device bindings
    static DSTATUS disk_initialize(void* disk_obj);
    static DRESULT disk_writep(void* disk_obj, const BYTE* buff, DWORD sc);
    static DRESULT disk_readp(void* disk_obj, BYTE* buff,
                              DWORD sector, UINT offset, UINT count);

    struct ctx_type
    {
        ctx_type() :pool{}, alloc{&pool}, fat{} { }
        // TODO: make it configurable, i.e. by moving it to the template arguments
        // Memory pool where fat objects will reside
        ecl::pool<get_alloc_blk_size(), 256> pool;
        // Will be rebound to a proper object type each time allocation will occur
        allocator   alloc;
        // Petite FAT object
        FATFS       fat;
    };

    static ecl::safe_storage<ctx_type> m_stor;

    static ctx_type &ctx() { return m_stor.get(); }
};

template<class Block>
ecl::safe_storage<typename petit<Block>::ctx_type> petit<Block>::m_stor;

//------------------------------------------------------------------------------

template<class Block>
fs::inode_ptr petit<Block>::mount()
{
    m_stor.init();

    auto rc = Block::init();
    if (is_error(rc)) {
        return nullptr;
    }

    // Pass block device bindings
    // that will be used by internal fat routines
    ctx().fat.disk = {
        nullptr,
        disk_initialize,
        disk_writep,
        disk_readp
    };

    // TODO: error check!
    auto res = pf_mount(&ctx().fat);
    if (res != FR_OK) {
        return nullptr;
    }

    auto iptr = ecl::allocate_shared<dir_inode>(ctx().alloc, &ctx().fat, ctx().alloc);
    iptr->set_weak(iptr);

    // TODO;
    return iptr;
}

template<class Block>
constexpr size_t petit<Block>::get_alloc_blk_size()
{
    // Determine a size of allocations.
    // The maximum size will be used as block size for the pool
    return alignof(std::max_align_t);
}

template<class Block>
DSTATUS petit<Block>::disk_initialize(void* disk_obj)
{
    (void)disk_obj;
    // Do nothing?
    return RES_OK;
}

template<class Block>
DRESULT petit<Block>::disk_writep(void* disk_obj, const BYTE* buff, DWORD sc)
{
    (void) disk_obj;

    size_t to_write = 512;

    if (!buff) {
        if (sc) {
            // Initiate write process
            // TODO: is sector size always will be 512 bytes?
            off_t offt = sc * to_write;
            Block::seek(offt);
        } else {
            // Finalize write process
            auto rc = Block::flush();
            if (is_error(rc)) {
                return RES_ERROR;
            }
        }
    } else {
        // Send data to the disk
        auto rc = Block::write(buff, to_write);
        if (is_error(rc) || to_write != 512) {
            return RES_ERROR;
        }
    }

    return RES_OK;
}

template<class Block>
DRESULT petit<Block>::disk_readp(void* disk_obj, BYTE* buff,
                                   DWORD sector, UINT offset, UINT count)
{
    (void)disk_obj;

    // TODO: what means "If a NULL is given [in buff], read data bytes
    // are to be forwarded to the outgoing stream instead of the memory.
    // How the streamimg data is dealed depends on each project."
    // See: http://elm-chan.org/fsw/ff/pf/dreadp.html

    size_t to_read = count;
    // TODO: is sector size always will be 512 bytes?
    off_t offt = sector * 512 + offset;
    Block::seek(offt);

    if (buff) {
        auto rc = Block::read(buff, to_read);
        if (is_error(rc) || to_read != count) {
            return RES_ERROR;
        }
    }

    return RES_OK;
}

//! @}

//! @}

//! @}

} // namespace fat

} // namespace ecl

#endif // FATFS_FILESYSTEM_HPP_

