#ifndef LIB_ALLOC_POOL_HPP_
#define LIB_ALLOC_POOL_HPP_

#include <ecl/assert.hpp>
#include <memory>
#include <array>

namespace ecl
{

// Type erasure
class pool_base
{
public:
    // Allocates memory suitable for type T, with respect to its aligment
    template< typename T >
    T* aligned_alloc(size_t n);

    // Deallocates memory
    template< typename T >
    void deallocate(T *p, size_t n);

protected:
    // Provided by a real pool
    // Allocates n items each size of obj_size with respect to
    // align
    virtual uint8_t* real_alloc(size_t n, size_t align, size_t obj_size) = 0;
    // Provided by a real pool
    // Deallocates n items each size of obj_size.
    // 'p' is a pointer to memory area, previously returned by real_alloc
    // function.
    virtual void real_dealloc(uint8_t *p, size_t n, size_t obj_size) = 0;
};

//------------------------------------------------------------------------------

template< typename T >
T* pool_base::aligned_alloc(size_t n)
{
    T *p = reinterpret_cast< T* > (real_alloc(n, alignof(T), sizeof(T)));

#if 0
    ecl::cout << "alloc " << n << " x " << sizeof(T) << " = "
              << n * sizeof(T) << " bytes from " << (int) p
              << ecl::endl;
#endif
    return p;
}

template< typename T >
void pool_base::deallocate(T *p, size_t n)
{
#if 0
    ecl::cout << "dealloc " << n << " x " << sizeof(T) << " = "
              << n * sizeof(T) << " bytes from " << (int) p
              << ecl::endl;
#endif
    real_dealloc(reinterpret_cast< uint8_t *>(p), n, sizeof(T));
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// TODO: comments
// Holds given buffer as a pool. Can be shared by different instances of
// an allocator
template< size_t blk_sz, size_t blk_cnt >
class pool : public pool_base
{
    // Sanity checks
    static_assert(blk_sz, "Block size must be bigger than 0");
    static_assert(blk_cnt, "Block count must be bigger than 0");
    static_assert(!(blk_sz & (blk_sz - 1)), "Block size must be power of two");

public:
    pool();

    // Allocates memory suitable for type T, with respect to its aligment
    uint8_t* real_alloc(size_t n, size_t align, size_t obj_sz) override;
    void real_dealloc(uint8_t *p, size_t n, size_t obj_sz) override;

private:
    static constexpr auto info_blks_sz();
    static constexpr auto data_blks_sz();

    void mark(size_t idx, bool used);
    // Check if the block is free or not.
    bool is_free(size_t idx) const;
    // Get a pointer of the particular block.
    uint8_t* get_block(size_t idx);

    // Data buffer must be aligned to size of block,
    // this will reduce complexity of calculations.
    // 64 is now maximum alignment supported TODO: clarify
    alignas((blk_sz > 64) ? 64 : blk_sz)
    std::array< uint8_t, data_blks_sz() >   m_data;
    std::array< uint8_t, info_blks_sz() >   m_info;
};

//------------------------------------------------------------------------------

template< size_t blk_sz, size_t blk_cnt >
pool< blk_sz, blk_cnt >::pool()
    :m_data{0}
    ,m_info{0}
{
}

template< size_t blk_sz, size_t blk_cnt >
uint8_t* pool< blk_sz, blk_cnt >::real_alloc(size_t n, size_t align, size_t obj_sz)
{
    // Consider reviewing the block size if this assertion fails.
    assert(align < blk_sz);
    // Not allowed to allocated zero-length buffer
    assert(n);

    // TODO: use byte-based iteration, instead of bit-based

    bool found = false;
    size_t i = 0;
    size_t j;

    // Convert a count of objects to a block count
    n = n * obj_sz / blk_sz + 1;

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
    assert(n);
    assert(p); // For now

    const uint8_t *start = m_data.begin();
    const uint8_t *end   = m_data.end();

    assert(p >= start && p < end);

    size_t idx = (p - start) / blk_sz;
    size_t cnt = (end - p)   / blk_sz;
    // Convert bytes to blocks
    n = n * obj_sz / blk_sz;

    assert(n <= cnt);

    while (n) {
        size_t to_free = idx + --n;
        assert(!is_free(to_free));
        mark(to_free, false);
    }
}



//------------------------------------------------------------------------------

template< size_t blk_sz, size_t blk_cnt >
constexpr auto pool< blk_sz, blk_cnt >::info_blks_sz()
{
    // Count bytes required to hold info bits. One bit per each block.
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
    assert(idx < blk_cnt);

    uint byte = idx >> 3;
    uint bit  = idx & 7;

    return !(m_info[byte] & (1 << bit));
}


template< size_t blk_sz, size_t blk_cnt >
void pool< blk_sz, blk_cnt >::mark(size_t idx, bool used)
{
    assert(idx < blk_cnt);

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
    assert(idx < blk_cnt);
    return m_data.begin() + idx * blk_sz;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

template< typename T >
class pool_allocator
{
public:
    pool_allocator(pool_base *pool);
    ~pool_allocator();

    T* allocate(size_t n);
    void deallocate(T *p, size_t n);

    // Create new instanse of thr allocator, suitable for a new type
    template< typename U >
    pool_allocator< U > rebind() const;

private:
    pool_base *m_pool;
};


template< typename T >
pool_allocator< T >::pool_allocator(pool_base *pool)
    :m_pool(pool)
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
