#ifndef FATFS_FILESYSTEM_HPP_
#define FATFS_FILESYSTEM_HPP_

#include <ecl/pool.hpp>
#include <fs/inode.hpp>

namespace fat
{

template< class Block >
class petite // TODO: rename it to 'petite_fat'
{
public:
    petite();
    ~petite();
    // Mounts a system and returns the root inode
    fs::inode_ptr mount();

private:
    static constexpr size_t get_alloc_blk_size();

    // Memory pool where inodes and descriptors will reside
    ecl::pool< get_alloc_blk_size(), 256 >          m_pool;
    // Will be rebound each time allocation will occur
    ecl::pool_allocator< uint8_t >                  m_alloc;
};


}

#endif // FATFS_FILESYSTEM_HPP_

