#include <fs/inode_ptr.hpp>
#include <fs/inode.hpp>

using namespace fs;

//------------------------------------------------------------------------------

inode_ptr::inode_ptr()
	:m_next(this)
	,m_inode(nullptr)
{

}

inode_ptr::inode_ptr(inode *i)
	:m_next(this)
	,m_inode(i)
{

}

inode_ptr::~inode_ptr()
{
	if (is_last() && m_inode)
		m_inode->release();
}

inode_ptr::inode_ptr(inode_ptr &other)
	:m_next(&other)
	,m_inode(other.m_inode)
{
	inode_ptr *prev = other.get_prev();
	prev->set_next(this);
}

inode_ptr::inode_ptr(inode_ptr &&other)
	:m_next(other.m_next)
	,m_inode(other.m_inode)
{
	other.m_next = &other;
	other.m_inode = nullptr;
}


inode_ptr& inode_ptr::operator=(inode_ptr &other)
{
	if (&other != this) {
		if (is_last()) {
			// Release the resource
			if (m_inode) {
				m_inode->release();
				m_inode = nullptr;
			}
		} else {
			// Unlink me
			inode_ptr *prev = get_prev();
			inode_ptr *next = get_next();

			prev->set_next(next);
		}

		m_inode = other.m_inode;

		// Link me to the new company
		other.set_prev(this);
		this->set_next(&other);
		m_inode = other.m_inode;
	}

	return *this;
}

inode_ptr* inode_ptr::get_next()
{
	return this->m_next;
}

inode_ptr* inode_ptr::get_prev()
{
	inode_ptr *cur = this->m_next;
	while (cur->m_next != this) {
		cur = cur->m_next;
	}

	return cur;
}


void inode_ptr::set_next(inode_ptr *ptr)
{
	this->m_next = ptr;
}

void inode_ptr::set_prev(inode_ptr *ptr)
{
	this->get_prev()->set_next(ptr);
}
