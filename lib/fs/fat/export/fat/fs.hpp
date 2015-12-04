#ifndef FATFS_FILESYSTEM_HPP_
#define FATFS_FILESYSTEM_HPP_

#include <ecl/pool.hpp>
#include <fs/inode.hpp>

#include "fat/fs.hpp"
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


    // Memory pool where inodes and descriptors will reside
    ecl::pool< get_alloc_blk_size(), 256 >   m_pool;
    // Will be rebound each time allocation will occur
    ecl::pool_allocator< uint8_t >           m_alloc;
    // Petite FAT object
    FATFS m_fat;
    // Block device on which FAT will operate
    Block m_block;
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
        (void *) &m_block,
        disk_initialize,
        disk_writep,
        disk_readp
    };

    // TODO: error check!
    pf_mount(&m_fat);

    // TODO;
    return fs::inode_ptr{};
}

template< class Block >
constexpr size_t petit< Block >::get_alloc_blk_size()
{
    // Determine a size of allocations.
    // The maximum size will be used as block size for the pool

    using finode_allocator = ecl::pool_allocator< file_inode >;
    constexpr size_t finode_blk_sz
            = ecl::shared_allocation_size< file_inode, finode_allocator >::value;

    using dinode_allocator = ecl::pool_allocator< dir_inode >;
    constexpr size_t dinode_blk_sz
            = ecl::shared_allocation_size< dir_inode, dinode_allocator >::value;

    //	using fd_allocator = ecl::pool_allocator< fs::file_descriptor >;
    //	constexpr size_t fd_blk_sz
    //    = ecl::shared_allocation_size< file_inode, fd_allocator >::value;

    //	using dd_allocator = ecl::pool_allocator< fs::dir_descriptor >;
    //	constexpr size_t dd_blk_sz
    //	= ecl::shared_allocation_size< fs::dir_descriptor, dd_allocator >::value;

    //	constexpr size_t max_size = dd_blk_sz > fd_blk_sz ? dd_blk_sz : fd_blk_sz;
    //	max_size = inode_blk_sz > max_size ? max_size : inode_blk_sz;

    constexpr size_t max_size = finode_blk_sz > dinode_blk_sz ?
                finode_blk_sz : dinode_blk_sz;

    return max_size;
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

    return RES_OK;
}


}

#endif // FATFS_FILESYSTEM_HPP_

