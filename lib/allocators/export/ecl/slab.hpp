#ifndef LIB_ALLOC_SLAB_HPP_
#define LIB_ALLOC_SLAB_HPP_

#include <ecl/assert.hpp>
#include <memory>

namespace ecl
{

// TODO: comments
// Holds given buffer as a slab. Can be shared by different instances of
// an allocator
template< class Cache >
struct slab
{
public:
    slab(uint8_t *slab, size_t slab_size);

    // Gets a maximum count of caches that may be stored.
    size_t get_max() const;
    // Gets start of the cache buffer.
    // Used for slab iteration.
    Cache* get_start() const;
    // Gets pointer to the cache item beyond the end of
    // the cache buffer. Used for slab iteration.
    const Cache* get_end() const;
    // Marks a chunk in a cache as used or free
    void mark(size_t idx, bool used);
    // Check if the cache is free or not
    bool is_free(size_t idx) const;
    // Get a pointer of the particular cache
    Cache* get_cache(size_t idx);

private:
    uint8_t *m_slab;
    size_t  m_slab_sz;
    size_t  m_info_sz;
    size_t  m_caches_sz;
};

template< class Cache >
slab< Cache >::slab(uint8_t *slab, size_t slab_size)
    :m_slab{slab}
    ,m_slab_sz{slab_size}
    ,m_info_sz{0}
    ,m_caches_sz{0}
{
    // TODO: assert alignment for slab
    assert(slab);
    assert(slab_size >= 2 * sizeof(Cache));

    void *to_align = m_slab;
    size_t sz = m_slab_sz;
    constexpr size_t cache_sz = sizeof(Cache);

    // Align a start of the slab, so it can hold cache objects
    void *ret = std::align(alignof(Cache), cache_sz, to_align, sz);
    assert(ret);
    (void) ret;

    // Save alignment
    m_slab = reinterpret_cast< uint8_t* >(to_align);

    // Info block size
    m_info_sz = m_slab_sz / (8 * cache_sz + 1) + 1;

    // Extend info size to cache size boundary
    m_info_sz = (m_info_sz - 1) / cache_sz * cache_sz + cache_sz;

    // Align cache size to cache size boundary
    m_caches_sz = (m_slab_sz - m_info_sz) / cache_sz * cache_sz;

    // Mark all caches as free
    std::fill_n(m_slab, m_info_sz, 0);
}

template< class Cache >
size_t slab< Cache >::get_max() const
{
    return m_caches_sz / sizeof(Cache);
}

template< class Cache >
Cache* slab< Cache >::get_start() const
{
    return reinterpret_cast< Cache* >(m_slab + m_info_sz);
}

template< class Cache >
const Cache* slab< Cache >::get_end() const
{
    return reinterpret_cast< const Cache* >(m_slab + m_info_sz + m_caches_sz);
}

template< class Cache >
void slab< Cache >::mark(size_t idx, bool used)
{
    assert(idx < get_max());

    uint byte = idx >> 3;
    uint bit  = idx & 7;

    if (used) {
        m_slab[byte] |= (1 << bit);
    } else {
        m_slab[byte] &= ~(1 << bit);
    }
}

template< class Cache >
bool slab< Cache >::is_free(size_t idx) const
{
    assert(idx < get_max());

    uint byte = idx >> 3;
    uint bit  = idx & 7;

    return !(m_slab[byte] & (1 << bit));
}

template< class Cache >
Cache* slab< Cache >::get_cache(size_t idx)
{
    assert(idx < get_max());
    return get_start() + idx;
}

// -----------------------------------------------------------------------------

// TODO: comments
template< class Cache >
class slab_allocator
{
public:
    slab_allocator(slab< Cache > *slab);
    ~slab_allocator();

    Cache* allocate(size_t n);
    void deallocate(Cache *p, size_t n);

private:
    slab< Cache > *m_slab;

};

template< class Cache >
slab_allocator< Cache >::slab_allocator(slab< Cache > *slab)
    :m_slab{slab}
{
}

template< class Cache >
slab_allocator< Cache >::~slab_allocator()
{
}

template< class Cache >
Cache* slab_allocator< Cache >::allocate(size_t n)
{
    assert(n);
    const size_t max = m_slab->get_max();
    bool found = false;
    size_t i = 0;
    size_t j;

    // Convert size to offset
    n--;

    // Iterate over whole slab
    for (; i + n < max; ++i) {
        j = i;

        // Finds cache row with appropriate length
        for (; j <= i + n; ++j) {
            if (!m_slab->is_free(j)) {
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
            m_slab->mark(j, true);
        }

        return m_slab->get_cache(i);
    }

    return nullptr;
}

template< class Cache >
void slab_allocator< Cache >::deallocate(Cache *p, size_t n)
{
    assert(n);
    assert(p); // For now

    const Cache *start = m_slab->get_start();
    const Cache *end   = m_slab->get_end();

    assert(p >= start && p < end);

    size_t idx = p - start;
    size_t cnt = end - p;
    assert(n <= cnt);

    while (n) {
        assert(!m_slab->is_free(idx));
        m_slab->mark(idx + --n, false);
    }
}

}


#endif // LIB_ALLOC_SLAB_HPP_
