#ifndef LIB_FS_INODE_PTR_HPP_
#define LIB_FS_INODE_PTR_HPP_


namespace fs
{

class inode;

class inode_ptr
{
public:
	inode_ptr();
	inode_ptr(inode *in);
	~inode_ptr();

	inode_ptr(inode_ptr &other);
	inode_ptr(inode_ptr &&other);
	inode_ptr& operator=(inode_ptr &other);

	// Returns true if this pointer holds the last remaining node
	bool is_last() const;

private:
	// Pointers are stored in the single-linked circular list
	inode_ptr *m_next;
	// Inode itself
	inode *m_inode;

	// List traverse helpers
	inode_ptr* get_next();
	inode_ptr* get_prev();

	void set_next(inode_ptr *ptr);
	void set_prev(inode_ptr *ptr);
};


}

#endif
