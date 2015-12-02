#ifndef LIB_FS_TYPES_HPP_
#define LIB_FS_TYPES_HPP_

#include <ecl/memory.hpp>

namespace fs
{
    class inode;
    class file_descriptor;
    class dir_descriptor;


    using inode_ptr = ecl::shared_ptr< inode >;
    using file_ptr = ecl::shared_ptr< file_descriptor >;
    using dir_ptr = ecl::shared_ptr< dir_descriptor >;
}

#endif
