#ifndef FATFS_DIR_INODE_HPP_
#define FATFS_DIR_INODE_HPP_

#include <sys/types.h>
#include <cstdint>
#include <fs/inode.hpp>


namespace fat
{

class dir_inode : fs::inode
{
public:
    using type = typename fs::inode::type;

    dir_inode();
    virtual ~dir_inode();
    virtual type get_type() const override;
    virtual fs::dir_ptr open_dir() override;
    virtual ssize_t size() const override;
    virtual ssize_t get_name(const char *buf, size_t buf_sz) const override;
};

}

#endif  // FATFS_DIR_INODE_HPP_
