#ifndef FATFS_FILESYSTEM_HPP_
#define FATFS_FILESYSTEM_HPP_

#include <ecl/pool.hpp>
#include <fs/inode.hpp>
#include <fat/fs.hpp>
#include <fat/file_inode.hpp>
#include <fat/dir_inode.hpp>

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
    static constexpr size_t get_alloc_blk_size();

    // Memory pool where inodes and descriptors will reside
    ecl::pool< get_alloc_blk_size(), 256 >          m_pool;
    // Will be rebound each time allocation will occur
    ecl::pool_allocator< uint8_t >                  m_alloc;
};

template< class Block >
petit< Block >::petit()
	:m_pool{}
	,m_alloc{&m_pool}
{

}

template< class Block >
petit< Block >::~petit()
{

}

template< class Block >
fs::inode_ptr petit< Block >::mount()
{
	return fs::inode_ptr{};
}

template< class Block >
constexpr size_t petit< Block >::get_alloc_blk_size()
{
	// Determine size of allocations.
	// Maximum size will be used as block size for the pool

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



}

#endif // FATFS_FILESYSTEM_HPP_

