#ifndef LIB_ECL_FS_FILESYSTEM_HPP_
#define LIB_ECL_FS_FILESYSTEM_HPP_

#include "inode.hpp"
#include <fs/inode.hpp>
#include <ecl/pool.hpp>

namespace fat
{

template< class Block >
class petite // TODO: rename it to 'petite_fat'
{
public:
    petite();
    // Mounts a system and returns the root inode
    fs::inode_ptr mount();

private:
    // Determine size of allocations
    using inode_allocator = ecl::pool_allocator< file_inode >;
    static constexpr size_t inode_blk_sz
    = ecl::shared_allocation_size< file_inode, inode_allocator >::value;

//    TODO
//    using fd_allocator = ecl::pool_allocator< file_descriptor >;
//    static constexpr size_t fd_blk_sz
//    = ecl::shared_allocation_size< file_descriptor, fd_allocator >::value;

    // Pool to store inodes
    ecl::pool< inode_blk_sz, 256 > m_alloc;
    // Pool to store file descriptors
    // TODO

    inode_allocator ialloc;
};


}

#endif // LIB_ECL_FS_FILESYSTEM_HPP_

