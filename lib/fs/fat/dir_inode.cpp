#include "fat/dir_inode.hpp"
#include "fat/dir.hpp"
#include <ecl/iostream.hpp>

using namespace fat;

dir_inode::dir_inode(FATFS *fs, const allocator &alloc, const char *path, const char *name)
    :m_alloc(alloc)
    ,m_path()
    ,m_fs(fs)
    ,my_ptr(nullptr)
{

    if (!path && !name) {
        m_path = allocate_path("/", nullptr, m_alloc);
    } else {
        m_path = allocate_path(path, name, m_alloc);
    }

    ecl::cout << "dirinode path: " << m_path->get_path() << ecl::endl;
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

    // KILLS STACK!
#if 0
    DIR fat_dir;
    FRESULT res = pf_opendir(m_fs, &fat_dir, m_path->get_path());

    assert(res == FR_OK);

    auto ptr = ecl::allocate_shared< dir, decltype(m_alloc) >(m_alloc, *my_ptr, fat_dir);
#endif

    return nullptr;
}

ssize_t dir_inode::size() const
{
    return -1;
}

ssize_t dir_inode::get_name(const char *buf, size_t buf_sz) const
{
    (void) buf;
    (void) buf_sz;
    return -1;
}

int dir_inode::set_weak(fs::inode_ptr *ptr)
{
    assert(!my_ptr);
    assert(ptr);
    my_ptr = ptr;
    return 0;
}
