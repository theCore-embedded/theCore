#ifndef FATFS_DIR_HPP_
#define FATFS_DIR_HPP_

#include <fs/dir_descriptor.hpp>
#include <fs/inode.hpp>

#include "src/pff.h"
#include "fat/types.hpp"
#include "../../path.hpp"

namespace fat
{

class dir : public fs::dir_descriptor
{
public:
    dir(const fs::inode_ptr &node, FATFS *fs, const allocator &alloc,
        const DIR &fat_dir, fat::path_ptr path);
    virtual ~dir();

    // Next entity in a dir
    // nullptr returned if no more items
    virtual fs::inode_ptr next() override;
    // Rewinds to the start of the dir
    // -1 if error, 0 otherwise
    virtual int rewind() override;

private:
    // TBD
    FATFS       *m_fs;
    allocator   m_alloc;
    DIR         m_fdir;
    path_ptr    m_path;
};


}

#endif
