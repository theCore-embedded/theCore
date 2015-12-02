#ifndef FATFS_DIR_HPP_
#define FATFS_DIR_HPP_

#include <fs/dir_descriptor.hpp>
#include <fs/inode.hpp>

namespace fat
{

class dir : public fs::dir_descriptor
{
public:
    dir(const fs::inode_ptr &node);
    ~dir();

    // Next entity in a dir
    // nullptr returned if no more items
    virtual fs::inode_ptr next() override;
    // Rewinds to the start of the dir
    // -1 if error, 0 otherwise
    virtual int rewind() override;

private:
    // TBD
};


}

#endif
