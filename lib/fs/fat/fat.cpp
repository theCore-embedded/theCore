#include <fat/fs.hpp>

using namespace fat;

template< class Block >
petite< Block >::petite()
    :m_pool{}
    ,m_alloc{&m_pool}
{

}

template< class Block >
petite< Block >::~petite()
{

}

template< class Block >
fs::inode_ptr petite< Block >::mount()
{
	return fs::inode_ptr{};
}

template< class Block >
constexpr size_t petite< Block >::get_alloc_blk_size()
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
