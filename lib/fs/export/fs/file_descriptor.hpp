/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LIB_FS_FILE_DESCRIPTOR_HPP_
#define LIB_FS_FILE_DESCRIPTOR_HPP_

#include <cstddef>
#include <cstdint>
#include <sys/types.h>

#include "types.hpp"

namespace fs
{

enum class seekdir
{
	beg,
	cur,
	end
};

class file_descriptor
{
public:
    file_descriptor(const inode_weak &node);
	virtual ~file_descriptor();

    virtual ssize_t read(uint8_t *buf, size_t size) = 0;
    virtual ssize_t write(const uint8_t *buf, size_t size) = 0;
    virtual int seek(off_t offt, seekdir way = seekdir::beg) = 0;
    virtual off_t tell() = 0;
    virtual int close() = 0;

protected:
	// Associated inode
    inode_ptr m_inode;

};

}

#endif
