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
    virtual ~file_inode();
    virtual type get_type() const override;
    virtual fs::file_ptr open() override;
	virtual ssize_t size() const override;
	virtual ssize_t get_name(const char *buf, size_t buf_sz) const override;

private:
    // TBD
};

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
#endif // LIB_ECL_FS_FILEHANDLE_HPP_
