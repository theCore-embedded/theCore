#ifndef FATFS_DIR_INODE_HPP_
#define FATFS_DIR_INODE_HPP_

#include <sys/types.h>
#include <fs/inode.hpp>
#include <cstdint>

#include "src/pff.h"

namespace fat
{

// Holds a reference to a path string and manages its deallocation
class path_handle;
using path_ptr = ecl::shared_ptr< path_handle >;


class dir_inode : public fs::inode
{
public:
    using type = typename fs::inode::type;

    dir_inode(FATFS *fs, path_ptr path);
    virtual ~dir_inode();
    virtual type get_type() const override;
    virtual fs::dir_ptr open_dir() override;
    virtual ssize_t size() const override;
    virtual ssize_t get_name(const char *buf, size_t buf_sz) const override;
private:

    FATFS         *m_fs;  // Filesystem object
    path_ptr      m_path; // Path of this inode
};

}

#endif  // FATFS_DIR_INODE_HPP_
