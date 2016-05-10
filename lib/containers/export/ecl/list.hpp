//! \file
//! \brief The intrusive list interface
//! \todo Examples
#ifndef ECL_INTRUSIVE_LIST_
#define ECL_INTRUSIVE_LIST_

#include <ecl/utils.hpp>

#include <cstddef>
#include <type_traits>

namespace ecl
{

//! Intrusive, double-linked, circular list node.
//! \details Any class can embed intrusive list node by composing it.
class list_node
{
public:
    //! Constructs emtpy list node.
    list_node();
    //! Destructs list node, unlinking itself from any list.
    ~list_node();

    //! Adds a node after this node.
    //! \param[in] Node to add in the list
    void add_after(list_node &node);

    //! Adds a node before this node.
    //! \param[in] Node to add in the list
    void add_before(list_node &node);

    //! Unlinks this node from a list.
    //! \details Has no effect if list is empty.
    void unlink();

    //! Checks if node is connected to any other node.
    bool linked() const;

    //! Returns a pointer to a next list node.
    list_node* next() const;

    //! Returns a pointer to a previous list node.
    list_node* prev() const;

private:
    list_node *m_next; //!< Next node in a list. Equals to this if the list is empty.
    list_node *m_prev; //!< Previous node in a list. Equals to this if the list is empty.
};

//------------------------------------------------------------------------------

inline list_node::list_node()
    :m_next{this}
    ,m_prev{this}
{

}

inline list_node::~list_node()
{
    unlink();
}

inline void list_node::add_after(list_node &node)
{
    node.m_next = m_next;
    node.m_prev = this;
    m_next->m_prev = &node;
    m_next = &node;
}

inline void list_node::add_before(list_node &node)
{
    node.m_prev = m_prev;
    node.m_next = this;
    m_prev->m_next = &node;
    m_prev = &node;
}

inline bool list_node::linked() const
{
    return m_next != this;
}

inline void list_node::unlink()
{
    m_next->m_prev = m_prev;
    m_prev->m_next = m_next;
    m_next = m_prev = this;
}

inline list_node* list_node::next() const
{
    return m_next;
}

inline list_node* list_node::prev() const
{
    return m_prev;
}

//------------------------------------------------------------------------------

template< typename T, list_node T::* Mptr >
class list_iter;

//! Intrusive, double-linked, circular list head.
//! \details Any class can use intrusive list node by composing it. \sa list_node
//! \tparam T     The type of enclosing class or struct.
//! \tparam Mptr  The member-pointer of the list node inside enclosing class.
template< typename T, list_node T::* Mptr >
class list
{
public:
    //! Returns iterator to first valid element of a list,
    //! provided that this node is a head.
    list_iter< T, Mptr > begin();

    //! Returns iterator to end of the list,
    //! provided that this node is a head.
    //! \warning Dereferencing end iterator is illegal
    list_iter< T, Mptr > end();

    //! Checks if list is empty.
    bool empty() const;

    //! Push element to the end of a list.
    void push_back(T &t);

private:
    list_node head; //!< List the first node, its head.
};

//------------------------------------------------------------------------------

template< typename T, list_node T::* Mptr >
list_iter< T, Mptr > list< T, Mptr >::begin()
{
    return list_iter< T, Mptr >{*head.next()};
}

template< typename T, list_node T::* Mptr >
list_iter< T, Mptr > list< T, Mptr >::end()
{
    return list_iter< T, Mptr >{head};
}

template< typename T, list_node T::* Mptr >
bool list< T, Mptr >::empty() const
{
    return !head.linked();
}

template< typename T, list_node T::* Mptr >
void list< T, Mptr >::push_back(T &t)
{
    head.add_after(t.*Mptr);
}

//------------------------------------------------------------------------------

//! Intrusive list's specific, safe iterator
//! \details It is safe to delete a node or an item and move this iterator after.
//! \tparam T     The type of enclosing class or struct.
//! \tparam Mptr  The member-pointer of the list node inside enclosing class.
template< typename T, list_node T::* Mptr >
class list_iter
{
public:
    //! Constructs iterator that iterates given list.
    list_iter(list_node &cur);

    //! Destroys iterator
    ~list_iter();

    //! Proceed to the next item in a list.
    list_iter& operator ++();
    //! Postfix version of the operatoris.
    list_iter operator ++(int);

    //! Accesses an object.
    T* operator ->();

    //! Acessess an object.
    T& operator *();

    //! Compares two iterators.
    //! \warning Comparing iterators from different lists is illegal.
    //! \return True if iterators point to the same object within the same list.
    bool operator ==(const list_iter& other);

    //! Compares two iterators.
    //! \warning Comparing iterators from different lists is illegal.
    //! \return False if iterators point to the same object within the same list.
    bool operator !=(const list_iter< T, Mptr >& other);

private:
    list_node *m_cur; //! Current list node.
    list_node *m_tmp; //! Temporary list node. Used for deleting nodes.
};

//------------------------------------------------------------------------------

template< typename T, list_node T::* Mptr >
list_iter< T, Mptr >::list_iter(list_node &cur)
    :m_cur{&cur}
    ,m_tmp{cur.next()}
{

}

template< typename T, list_node T::* Mptr >
list_iter< T, Mptr >::~list_iter()
{

}

template< typename T, list_node T::* Mptr >
list_iter< T, Mptr >& list_iter< T, Mptr >::operator ++()
{
    // Advance pointer. The current node has been possibly deleted.
    m_cur = m_tmp;
    m_tmp = m_tmp->next();
    return *this;
}
template< typename T, list_node T::* Mptr >
list_iter< T, Mptr > list_iter< T, Mptr >::operator ++(int)
{
    list_iter tmp{*this};
    ++*this;
    return tmp;
}

template< typename T, list_node T::* Mptr >
T& list_iter< T, Mptr >::operator *()
{
    return *(operator->());
}

template< typename T, list_node T::* Mptr >
T* list_iter< T, Mptr >::operator ->()
{
    auto offt = offset_of(Mptr);
    return reinterpret_cast< T* >(reinterpret_cast< uint8_t* >(m_cur) - offt);
}

template< typename T, list_node T::* Mptr >
bool list_iter< T, Mptr >::operator ==(const list_iter &other)
{
    return m_cur == other.m_cur;
}

template< typename T, list_node T::* Mptr >
bool list_iter< T, Mptr >::operator !=(const list_iter& other)
{
    return m_cur != other.m_cur;
}

} // namespace ecl

#endif // ECL_INTRUSIVE_LIST_
