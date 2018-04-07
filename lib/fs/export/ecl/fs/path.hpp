/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Filesystem path utilities

#ifndef LIB_FS_PATH_HPP_
#define LIB_FS_PATH_HPP_

#include <ecl/memory.hpp>
#include <string.h>

namespace ecl
{

namespace fs
{

//! \addtogroup lib Libraries and utilities
//! @{

//! \defgroup fs Filesystem support
//! @{

//! \defgroup fs_iface Filesystem interfaces
//! @{

class path_handle;

//! Alias to path smart pointer.
using path_ptr = ecl::shared_ptr<path_handle>;

//! Abstract path handle.
class path_handle
{
public:
    //! Gets path string.
    virtual const char* get_path() const = 0;
    //! Destroys path string.
    virtual ~path_handle() { }
};

//! Holds path and manages its deallocation.
template<class Alloc>
class path_erased : public path_handle
{
public:
    //! Owns given path, allocated by given allocator
    //! \details Allocation size is an amount of space allocated for the path.
    //! \param[in] path String representing a path.
    //! \param[in] alloc_sz Size of path string.
    //! \param[in] alloc Allocator that was used to allocate given path.
    path_erased(const char *path, size_t alloc_sz, const Alloc &alloc);

    //! Destroys path.
    virtual ~path_erased();

    //! \copydoc path_handle::get_path()
    virtual const char* get_path() const override;

    path_erased &operator=(path_erased&) = delete;
    path_erased(const path_erased&) = delete;

private:
    const char*   m_path;   //!< Path itself.
    size_t        m_sz;     //!< Path buffer size.
    Alloc         m_alloc;  //!< Allocator for the path.
};

//------------------------------------------------------------------------------

template<class Alloc>
path_erased<Alloc>::path_erased(const char *path, size_t alloc_sz, const Alloc &alloc)
    :m_path{path}
    ,m_sz{alloc_sz}
    ,m_alloc{alloc}
{
}

template<class Alloc>
path_erased<Alloc>::~path_erased()
{
    m_alloc.deallocate(const_cast< char* >(m_path), m_sz);
}

template<class Alloc>
const char* path_erased<Alloc>::get_path() const
{
    return m_path;
}

//------------------------------------------------------------------------------

//! Allocates new path, concatenating with tail, if needed.
//! \tparam Alloc Allocator type.
//! \param[in] path  Base path.
//! \param[in] tail  If not null, then this string will be added to
//!                  the end of the path. Trailing '/' will be added, if needed.
//! \param[in] alloc Allocator object.
//! \return pointer to the new path.
template<class Alloc>
path_ptr allocate_path(const char *path, const char *tail, const Alloc &alloc)
{
    ecl_assert(path);

    auto    char_alloc      = alloc.template rebind< char >();
    using   path_container  = path_erased<decltype(char_alloc)>;
    size_t  buf_sz          = strlen(path) + 1;
    bool    trailed         = (path[buf_sz - 2] == '/'); // Check if path ends with slash
    size_t  tail_sz         = (tail ? strlen(tail) : 0) + 1;
    char    *path_buf       = char_alloc.allocate(buf_sz + tail_sz + !trailed);

    // Allocation failed, let upper layers handle it.
    if (!path_buf) {
        return nullptr;
    }

    std::copy(path, path + buf_sz, path_buf);

    if (!trailed) {
        // Overwrite null terminator
        path_buf[buf_sz - 1] = '/';
        // Advance pointer to prevent tail from overwriting slash
        buf_sz++;
    }

    if (tail) {
        // Overwrite null terminator
        buf_sz--;
        std::copy(tail, tail + tail_sz, path_buf + buf_sz);
        buf_sz += tail_sz;
    }

    path_buf[buf_sz] = 0;

    // Idea is to use the same allocator both for path and for
    // a container, holding path
    auto ptr = ecl::allocate_shared
            < path_container, Alloc >(alloc, path_buf, buf_sz, char_alloc);

    return ptr;
}

//------------------------------------------------------------------------------

//! Path iterator.
class path_iter
{
public:
    //! Alias to path segment.
    //! \details First element of pair is the start of the segment,
    //! second element - the end of the segment.
    using segment = std::pair<const char *, const char *>;

    //! Constructs path iterator to be used with given path.
    //! \param[in] path Valid path, must not be null.
    path_iter(const char *path)
        :path{path}
        ,start{find_start(path)}
        ,end{find_end(start)}
    {

    }

    //! Gets next path segment.
    //! \param[out] seg Next path segment.
    //! \retval true Path segment found.
    //! \retval false No more path segments.
    bool next(segment &seg)
    {
        // No more tokens, we're at the end of a string
        if (!*start) {
            return false;
        }

        seg.first = start;
        seg.second = end;

        start = find_start(end);
        end = find_end(start);

        return true;
    }

private:
    //! Returns start of the path segment in the given string.
    const char *find_start(const char *from)
    {
        // Multiple '/' can be in path.
        // Skip them to find a start of the segment
        while (*from == '/') {
            from++;
        }

        return from;
    }

    //! Returns ends of the path segment in the given string.
    const char *find_end(const char *from)
    {
        while (*from && *from != '/') {
            from++;
        }

        return from;
    }

    const char  *path;      //!< Path itself.
    const char  *start;     //!< Last segment's start.
    const char  *end;       //!< Last segment's end.
};

//! @}

//! @}

//! @}

} // namespace fs

} // namespace ecl

#endif // LIB_FS_PATH_HPP_
