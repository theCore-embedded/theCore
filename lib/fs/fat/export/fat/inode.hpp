#ifndef LIB_ECL_FS_FILEHANDLE_HPP_
#define LIB_ECL_FS_FILEHANDLE_HPP_

#include <sys/types.h>
#include <cstdint>
#include <fs/inode.hpp>

namespace fat
{

class file_inode : fs::inode
{
public:
    using type = typename fs::inode::type;

    file_inode();
    virtual ~file_inode() override;
    virtual type get_type() const override;
    virtual fs::file_ptr open() override;

private:
    // TBD
};

}
#endif // LIB_ECL_FS_FILEHANDLE_HPP_
