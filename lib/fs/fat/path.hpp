// TODO: move it to the FS module rather than FATFS?
#ifndef FATFS_PATH_HPP_
#define FATFS_PATH_HPP_

#include <string.h>

namespace fat
{

class path_handle;
using path_ptr = ecl::shared_ptr< path_handle >;

class path_handle
{
public:
    virtual const char* get_path() const = 0;
    virtual ~path_handle() { }
};

// Holds path and manages its deallocation
template< class Alloc >
class path_erased : public path_handle
{
public:
    // Owns given path, allocated by given allocator
    // Allocation size is an amount of space allocated for the path
    path_erased(const char *path, size_t alloc_sz, const Alloc &alloc);
    virtual ~path_erased();

    // Returns path
    virtual const char* get_path() const override;

private:
    const char*   m_path;   // Path
    size_t        m_sz;     // Path buffer size
    Alloc         m_alloc;  // Allocator for the path
};


template< class Alloc >
path_erased< Alloc >::path_erased(const char *path, size_t alloc_sz, const Alloc &alloc)
    :m_path{path}
    ,m_sz{alloc_sz}
    ,m_alloc{alloc}
{
}

template< class Alloc >
path_erased< Alloc >::~path_erased()
{
    m_alloc.deallocate(const_cast< char* >(m_path), m_sz);
}

template< class Alloc >
const char* path_erased< Alloc >::get_path() const
{
    return m_path;
}


//------------------------------------------------------------------------------

// Allocates new path. If tail is not null then it will be added to
// the end of resulting path
template< class Alloc >
path_ptr allocate_path(const char *path, const char *tail, const Alloc &alloc)
{
    assert(path);

    auto    char_alloc      = alloc.template rebind< char >();
    using   path_container  = path_erased< decltype(char_alloc) >;
    size_t  buf_sz          = strlen(path) + 1;
    size_t  tail_sz         = tail ? strlen(tail) : 0 + 1;
    char    *path_buf       = char_alloc.allocate(buf_sz + tail_sz);

    assert(path_buf);

    std::copy(path, path + buf_sz, path_buf);
    if (tail) {
        // Overwrite null terminator
        std::copy(tail, tail + tail_sz, path_buf + buf_sz - 1);
        buf_sz += tail_sz;
    }

    path_buf[buf_sz] = 0;

    // Idea is to use the same allocator both for path and for
    // a container, holding path
    auto ptr = ecl::allocate_shared
            < path_container, Alloc >(alloc, path_buf, buf_sz, char_alloc);

    return ptr;
}


}

#endif
