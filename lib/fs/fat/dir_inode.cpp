#include "fat/dir_inode.hpp"
#include "fat/dir.hpp"
#include <ecl/iostream.hpp>

using namespace fat;

dir_inode::dir_inode(FATFS *fs, const allocator &alloc, const char *path, const char *name)
    :fs::inode()
    ,m_alloc(alloc)
    ,m_path()
    ,m_fs(fs)
{
    if (!path && !name) {
        m_path = allocate_path("/", nullptr, m_alloc);
    } else {
        m_path = allocate_path(path, name, m_alloc);
    }
}

dir_inode::~dir_inode()
{
}

dir_inode::type dir_inode::get_type() const
{
    return dir_inode::type::dir;
}

fs::dir_ptr dir_inode::open_dir()
{

    ecl_assert(!my_ptr.expired());
    DIR fat_dir;

    FRESULT res = pf_opendir(m_fs, &fat_dir, m_path->get_path());
    // TODO: graceful error handing
    // ecl_assert(res == FR_OK);
    if (res != FR_OK) {
        ecl::cout << "Error opening dir: " << m_path->get_path()
                  << " : " << res << ecl::endl;
        return nullptr;
    }

    auto inode = my_ptr.lock();
    ecl_assert(inode);

    auto ptr = ecl::allocate_shared< dir, decltype(m_alloc) >
            (m_alloc, inode, m_fs, m_alloc, fat_dir, m_path);

    return ptr;
}

ssize_t dir_inode::size() const
{
    // TODO
    return -1;
}

ssize_t dir_inode::get_name(char *buf, size_t buf_sz) const
{
    ecl_assert(buf);
    ecl_assert(buf_sz);

    const char *path = m_path->get_path();

    size_t path_len = strlen(path);
    const char *start = path;
    const char *end;

    //assert(path[path_len - 1] == '/');
    end = path + path_len;

    // Skip trailing '/'
    for (uint i = path_len - 1; i > 0; --i) {
        if (path[i - 1] == '/') {
            start = path + i; // Skipping forward slash
            break;
        }
    }

    // Reserve place for null terminator
    size_t to_copy = std::min(buf_sz - 1, (size_t) (end - start));

    // Algorithm check
    ecl_assert(to_copy < buf_sz);
    ecl_assert(start < end);

    buf[to_copy] = 0;

    std::copy(start, end, buf);

    return path_len;
}

