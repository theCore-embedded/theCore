//!
//! \file
//! \brief Memory pool and corresponding allocator facilities.
//! \todo Usage examples
//!
#ifndef LIB_ALLOC_POOL_HPP_
#define LIB_ALLOC_POOL_HPP_

#include <ecl/assert.h>
#include <ecl/types.h>

#if defined (POOL_ALLOC_TEST_PRINT_STATS) || defined (POOL_ALLOC_TEST_PRINT_EXTENDED_STATS)
#include <ecl/iostream.hpp>
#endif

#include <memory>
#include <array>

namespace ecl
{

//!
//! \brief Base class for memory pool.
//!
//! Provides interfaces for allocation and deallocation of objects of given type.
//! This abstraction helps split handing of type requirement and memory
//! allocations into separate entities.
//! \sa ecl::pool
//!
class pool_base
{
public:
    //!
    //! \brief Allocates memory suitable for type T, with respect to its aligment.
    //! This method will allocate continuous block of memory that suitable
    //! for given type and count of objects of given type. Start address of that
    //! block will be aligned with respect to the given type.
    //! \tparam    T  A type whose object will be placed in the allocated memory.
    //! \param[in] n  Count of objects that will be placed in the allocated memory.
    //!               Behavior is undefined if given count is equal to 0.
    //! \return nullptr if there is no space for given type and count in the pool.
    //!          Valid pointer returned otherwise.
    //!
    template< typename T >
    T* aligned_alloc(size_t n);

    //!
    //! \brief Deallocates memory, previously allocated by aligned_alloc.
    //! \pre  Memory pool with allocated block of given type,
    //!       starting from given address and with total size
    //!       that corresponds to amount of objects that where requested
    //!       in call to aligned_alloc()
    //! \post Memory pool with deallocated (invalidated) block in
    //!       given address with given object count and object type.
    //! \tparam    T A type whose object was placed in the pool memory.
    //!              block was originally allocated for other type.
    //!              Undefined behavior otherwise.
    //! \param[in] p A pointer to a block of previously allocated memory.
    //!              If given pointer is outside of pool or it points
    //!              to a block that wasn't allocated by alligned_alloc()
    //!              then behavior is undefined.
    //! \param[in] n Count of objects to deallocate. Must match the value
    //!              previously passed to alligned_alloc().
    //!              Undefined behavior otherwise.
    template< typename T >
    void deallocate(T *p, size_t n);

    //!
    //! Destroys a pool.
    //!
    virtual ~pool_base();

protected:
    //!
    //! \brief Allocates chunk with respect to given alingment.
    //! \param[in] n        Object count.
    //! \param[in] align    Required align.
    //! \param[in] obj_size Size of single object.
    //! \return  nullptr if there is no space for given type and count in the pool.
    //!          Valid pointer returned otherwise.
    //! \sa aligned_alloc()
    //!
    virtual uint8_t* real_alloc(size_t n, size_t align, size_t obj_size) = 0;

    //!
    //! \brief Deallocates chunk of given address and length.
    //! Length of chunk is determined by multiplication count of objects
    //! by object size.
    //! \param[in] p        Address of a chunk. Must be valid address, returned
    //!                     Previously by real_alloc().
    //! \param[in] n        Objects count. Must be the same as passed to
    //!                     real_alloc() when given chunk was allocated.
    //! \param[in] obj_size Size of single object. Must be the same as passed to
    //!                     real_alloc() when given chunk was allocated.
    //! \sa deallocate()
    //!
    virtual void real_dealloc(uint8_t *p, size_t n, size_t obj_size) = 0;
};

//------------------------------------------------------------------------------

template< typename T >
T* pool_base::aligned_alloc(size_t n)
{
    T *p = reinterpret_cast< T* > (real_alloc(n, alignof(T), sizeof(T)));

#ifdef POOL_ALLOC_TEST_PRINT_EXTENDED_STATS
    ecl::cout << "alloc " << (int)n << " x " << (int)sizeof(T) << " = "
              << (int)(n * sizeof(T)) << " bytes from "
              << static_cast< int >((uintptr_t)p) << ecl::endl;
#endif
    return p;
}

template< typename T >
void pool_base::deallocate(T *p, size_t n)
{
#ifdef POOL_ALLOC_TEST_PRINT_EXTENDED_STATS
    ecl::cout << "dealloc " << (int)n << " x " << (int)sizeof(T) << " = "
              << (int)(n * sizeof(T)) << " bytes from "
              << static_cast< int >((uintptr_t)p) << ecl::endl;
#endif
    real_dealloc(reinterpret_cast< uint8_t *>(p), n, sizeof(T));
}

//------------------------------------------------------------------------------

//!
//! \brief Holds given buffer as a pool.
//!
//! Object of this pool can be shared by different instances of an allocator.
//! Refer to a wiki for further info:
//! [here](https://en.wikipedia.org/wiki/Fixed-size_blocks_allocation)
//!
template< size_t blk_sz, size_t blk_cnt >
class pool : public pool_base
{
    // Sanity checks
    static_assert(blk_sz, "Block size must be bigger than 0");
    static_assert(blk_cnt, "Block count must be bigger than 0");
    static_assert(!(blk_sz & (blk_sz - 1)), "Block size must be power of two");

public:
    //! \brief Constructs pool.
    pool();

    //! \brief Destructs pool.
    ~pool();

    //! \copydoc pool_base::real_alloc()
    uint8_t* real_alloc(size_t n, size_t align, size_t obj_sz) override;

    //! \copydoc pool_base::real_dealloc()
    void real_dealloc(uint8_t *p, size_t n, size_t obj_sz) override;

#ifdef POOL_ALLOC_TEST
    // Special routines used for test purposes only
    auto& get_data() { return m_data; }
    auto& get_info() { return m_info; }
    // Debug routine
    void print_stats() const;
#endif

private:
    //! Gets size of info array containing bits representing data chunk state.
    static constexpr auto info_blks_sz();
    //! Gets size of data array.
    static constexpr auto data_blks_sz();

    //!
    //! \brief Marks block with given index as used\unused.
    //! \param[in] idx  Block index. Must be valid index of a block within a pool.
    //! \param[in] used True if block must be marked as used. False - if as unused.
    //!
    void mark(size_t idx, bool used);

    //!
    //! \brief Checks if block with given index is free or not.
    //! \param[in] idx  Block index. Must be valid index of a block within a pool.
    //! \return true if block is free.
    //!
    bool is_free(size_t idx) const;

    //!
    //! \brief Obtains a block by given index.
    //! \param[in] idx  Block index. Must be valid index of a block within a pool.
    //! \return Pointer to a block.
    //!
    uint8_t* get_block(size_t idx);

    // Data buffer must be aligned to size of block,
    // this will reduce complexity of calculations.
    // 64 is now maximum alignment supported TODO: clarify
    alignas((blk_sz > 64) ? 64 : blk_sz)
    std::array< uint8_t, data_blks_sz() >   m_data; //!< Memory pool.
    //! \todo use bitset?
    std::array< uint8_t, info_blks_sz() >   m_info; //!< Memory info array.
};

//------------------------------------------------------------------------------

template< size_t blk_sz, size_t blk_cnt >
pool< blk_sz, blk_cnt >::pool()
    :m_data{0}
    ,m_info{0}
{
}

template< size_t blk_sz, size_t blk_cnt >
pool< blk_sz, blk_cnt >::~pool()
{
}

template< size_t blk_sz, size_t blk_cnt >
uint8_t* pool< blk_sz, blk_cnt >::real_alloc(size_t n, size_t align, size_t obj_sz)
{
    // Consider reviewing the block size if this assertion fails.
    ecl_assert(align < blk_sz);
    // Not allowed to allocate zero-length buffer
    ecl_assert(n);

    // TODO: use byte-based iteration, instead of bit-based

    bool found = false;
    size_t i = 0;
    size_t j;

    // Convert a count of objects to a block count with rounding away from zero
    // to a boundary of the block size.
    n = (n * obj_sz + blk_sz - 1) / blk_sz;

    // Convert count to offset
    n--;

    // Iterate over whole slab
    for (; i + n < blk_cnt; ++i) {
        j = i;

        // Finds cache row with appropriate length
        for (; j <= i + n; ++j) {
            if (!is_free(j)) {
                break;
            }
        }

        if (j == i + n + 1) {
            found = true;
            break;
        }
    }

    if (found) {
        // Mark caches as used
        for (j = i; j <= i + n; ++j) {
            mark(j, true);
        }

        return get_block(i);
    }

    return nullptr;
}


template< size_t blk_sz, size_t blk_cnt >
void pool< blk_sz, blk_cnt >::real_dealloc(uint8_t *p, size_t n, size_t obj_sz)
{
    ecl_assert(n);
    ecl_assert(p); // For now

    auto start = m_data.begin();
    auto end   = m_data.end();

    ecl_assert(p >= start && p < end);

    size_t idx = (p - start) / blk_sz;
    size_t cnt = (end - p)   / blk_sz;

    // Convert a count of objects to a block count with rounding away from zero
    // to a boundary of the block size.
    n = (n * obj_sz + blk_sz - 1) / blk_sz;

    ecl_assert(n <= cnt);

    while (n) {
        size_t to_free = idx + --n;
        ecl_assert(!is_free(to_free));
        mark(to_free, false);
    }
}



//------------------------------------------------------------------------------

template< size_t blk_sz, size_t blk_cnt >
constexpr auto pool< blk_sz, blk_cnt >::info_blks_sz()
{
    // Count bytes required to hold info bits. One bit per each block.
    // TODO: use bitset?
    return blk_cnt / 8 + 1;
}

template< size_t blk_sz, size_t blk_cnt >
constexpr auto pool<blk_sz, blk_cnt>::data_blks_sz()
{
    return blk_cnt * blk_sz;
}

template< size_t blk_sz, size_t blk_cnt >
bool pool< blk_sz, blk_cnt >::is_free(size_t idx) const
{
    // TODO: use bitset?
    ecl_assert(idx < blk_cnt);

    uint byte = idx >> 3;
    uint bit  = idx & 7;

    return !(m_info[byte] & (1 << bit));
}


template< size_t blk_sz, size_t blk_cnt >
void pool< blk_sz, blk_cnt >::mark(size_t idx, bool used)
{
    // TODO: use bitset?
    ecl_assert(idx < blk_cnt);

    uint byte = idx >> 3;
    uint bit  = idx & 7;

    if (used) {
        m_info[byte] |= (1 << bit);
    } else {
        m_info[byte] &= ~(1 << bit);
    }
}

template< size_t blk_sz, size_t blk_cnt >
uint8_t *pool< blk_sz, blk_cnt >::get_block(size_t idx)
{
    ecl_assert(idx < blk_cnt);
    return m_data.begin() + idx * blk_sz;
}

#ifdef POOL_ALLOC_TEST_PRINT_STATS
template< size_t blk_sz, size_t blk_cnt >
void pool< blk_sz, blk_cnt >::print_stats() const
{
    // Print memory stats for whole pool
    size_t i = 0;
    size_t used = 0;
    size_t streak = 0;
    size_t max_streak = 0;
    for (i = 0; i < blk_cnt; ++i) {
        if (!is_free(i)) {
            ecl::cout << 'x';
            used++;
            max_streak = std::min(streak, max_streak);
            streak = 0;
        } else {
            ecl::cout << 'o';
            streak++;
        }
    }

    ecl::cout << "\n\n";

    ecl::cout << "_________________________" << ecl::endl;
    ecl::cout << "total blk:\t\t"        << (int) blk_cnt << ecl::endl;
    ecl::cout << "blk sz:\t\t\t"         << (int) blk_sz << ecl::endl;
    ecl::cout << "used:\t\t\t"           << (int) used << ecl::endl;
    ecl::cout << "longest free streak:\t"<< (int) streak << ecl::endl;
}
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//!
//! \brief Almost stateless pool allocator.
//!
//! Provides the interface for shared memory pool.
//! \tparam Type for whom memory should be allocated.
//!
template< typename T >
class pool_allocator
{
public:
    //!
    //! \brief Constructs pool allocator with memory pool given.
    //! \param[in] Memory pool to use with this allocator.
    //!
    pool_allocator(pool_base *pool);

    //! Deallocates allocator.
    ~pool_allocator();

    //!
    //! \brief Allocates memory for given object count.
    //!
    //! \warn This will not call object constructors.
    //! Memory will have alingment requirements of given type.
    //! \param[in] n Objects count.
    //! \return Valid pointer to an uninitilized memory if allocation sucessed.
    //!         Nullptr otherwise.
    //!
    T* allocate(size_t n);

    //! \todo: fill docs
    void deallocate(T *p, size_t n);

    //! \brief Create new instanse of the allocator, suitable for a new type
    //! \tparam U A new allocator type.
    template< typename U >
    pool_allocator< U > rebind() const;

private:
    pool_base *m_pool; //!< Memory pool. \todo use reference instead of pointer.
};


template< typename T >
pool_allocator< T >::pool_allocator(pool_base *pool)
    :m_pool{pool}
{
}

template< typename T >
pool_allocator< T >::~pool_allocator()
{
}

template< typename T >
T* pool_allocator< T >::allocate(size_t n)
{
    return m_pool->aligned_alloc< T >(n);
}

template< typename T >
void pool_allocator< T >::deallocate(T *p, size_t n)
{
    m_pool->deallocate(p, n);
}

template< typename T >
template< typename U >
pool_allocator< U > pool_allocator< T >::rebind() const
{
    return pool_allocator< U >{ m_pool };
}


}


#endif // LIB_ALLOC_POOL_HPP_
