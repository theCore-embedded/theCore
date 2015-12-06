#ifndef FATFS_FILESYSTEM_HPP_
#define FATFS_FILESYSTEM_HPP_

#include <ecl/pool.hpp>
#include <fs/inode.hpp>

#include "fat/types.hpp"
#include "fat/file_inode.hpp"
#include "fat/dir_inode.hpp"
#include "src/pff.h"

namespace fat
{

template< class Block >
class petit // TODO: rename it to 'petite_fat'
{
public:
    petit();
    ~petit();
    // Mounts a system and returns the root inode
    fs::inode_ptr mount();

private:
    // Get an estimated single allocation size
    // that will be used to store inodes.
    static constexpr size_t get_alloc_blk_size();

    // Block device bindings
    static DSTATUS disk_initialize(void* disk_obj);
    static DRESULT disk_writep(void* disk_obj, const BYTE* buff, DWORD sc);
    static DRESULT disk_readp(void* disk_obj, BYTE* buff,
                              DWORD sector, UINT offser, UINT count);


    // TODO: make it configurable, i.e. by moving it to the template arguments
    // Memory pool where fat objects will reside
    ecl::pool< get_alloc_blk_size(), 256 > m_pool;
    // Will be rebound to a proper object type each time allocation will occur
    allocator   m_alloc;
    // Petite FAT object
    FATFS       m_fat;
    // Block device on which FAT will operate
    Block       m_block;
};

template< class Block >
petit< Block >::petit()
    :m_pool{}
    ,m_alloc{&m_pool}
    ,m_fat{}
    ,m_block{}
{
}

template< class Block >
petit< Block >::~petit()
{

}

template< class Block >
fs::inode_ptr petit< Block >::mount()
{
    m_block.init();
    m_block.open();

    // Pass block device bindings
    // that will be used by internal fat routines
    m_fat.disk = {
        reinterpret_cast< void * >(&m_block),
        disk_initialize,
        disk_writep,
        disk_readp
    };

    // TODO: error check!
    auto res = pf_mount(&m_fat);
    if (res == FR_OK) {
        ecl::cout << "Mounted!" << ecl::endl;
    } else {
        ecl::cout << "Mount error: " << res << ecl::endl;
    }

    auto iptr = ecl::allocate_shared< dir_inode >(m_alloc, &m_fat, m_alloc);

    // TODO;
    return iptr;
}

template< class Block >
constexpr size_t petit< Block >::get_alloc_blk_size()
{
    // Determine a size of allocations.
    // The maximum size will be used as block size for the pool

    using finode_allocator = ecl::pool_allocator< file_inode >;
    constexpr uint16_t finode_blk_sz
            = ecl::shared_allocation_size< file_inode, finode_allocator >::value;

    using dinode_allocator = ecl::pool_allocator< dir_inode >;
    constexpr uint16_t dinode_blk_sz
            = ecl::shared_allocation_size< dir_inode, dinode_allocator >::value;

    //	using fd_allocator = ecl::pool_allocator< fs::file_descriptor >;
    //	constexpr size_t fd_blk_sz
    //    = ecl::shared_allocation_size< file_inode, fd_allocator >::value;

    //	using dd_allocator = ecl::pool_allocator< fs::dir_descriptor >;
    //	constexpr size_t dd_blk_sz
    //	= ecl::shared_allocation_size< fs::dir_descriptor, dd_allocator >::value;

    //	constexpr size_t max_size = dd_blk_sz > fd_blk_sz ? dd_blk_sz : fd_blk_sz;
    //	max_size = inode_blk_sz > max_size ? max_size : inode_blk_sz;

    constexpr uint16_t max_size = finode_blk_sz > dinode_blk_sz ?
                finode_blk_sz : dinode_blk_sz;

    if (!(max_size & (max_size - 1)))
        return max_size;

    // Align to the next power of 2
    constexpr auto bits_in_int = sizeof(int) * 8;
    constexpr auto power = bits_in_int - __builtin_clz(max_size);
    return 1 << power;
}

template< class Block >
DSTATUS petit< Block >::disk_initialize(void* disk_obj)
{
    Block *device = reinterpret_cast< Block* >(disk_obj);
    // Do nothing?
    assert(device);
    return RES_OK;
}


template< class Block >
DRESULT petit< Block >::disk_writep(void* disk_obj, const BYTE* buff, DWORD sc)
{
    (void) buff;
    (void) sc;
    Block *device = reinterpret_cast< Block* >(disk_obj);
    assert(device);

    return RES_OK;
}


template< class Block >
DRESULT petit< Block >::disk_readp(void* disk_obj, BYTE* buff,
                                   DWORD sector, UINT offser, UINT count)
{
    Block *device = reinterpret_cast< Block* >(disk_obj);
    assert(device);


    if (buff) {
        // TODO: is sector size always will be 512 bytes
        off_t offt = sector * 512 + offser;
        device->seek(offt);
        ssize_t read = device->read(buff, count);
        if (read < 0 || (size_t)read != count) {
            return RES_ERROR;
        }
    }


    return RES_OK;
}


}

#endif // FATFS_FILESYSTEM_HPP_

