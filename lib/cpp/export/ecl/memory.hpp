#ifndef ECL_MEMORY_HPP_
#define ECL_MEMORY_HPP_

namespace ecl
{

template< typename T, class Alloc >
class shared_ptr
{
public:
       //shared_ptr();
        shared_ptr(T *ptr,  Alloc d);
        ~shared_ptr();

        shared_ptr(shared_ptr &other);
        shared_ptr(shared_ptr &&other);
        shared_ptr& operator=(shared_ptr &other);

        // Returns true if this pointer holds the last remaining node
        bool unique() const;

        // Returs a value itself
        T* get() const;

private:
        // Pointers are stored in the single-linked circular list
        shared_ptr *m_next;
        // An object itself
        T          *m_object;
        // Allocator fot he object
        Alloc      m_alloc;

        // List traverse helpers
        shared_ptr* get_next() const;
        shared_ptr* get_prev() const;

        void set_next(shared_ptr *ptr);
        void set_prev(shared_ptr *ptr);
};


//------------------------------------------------------------------------------

template< typename T, class Alloc >
shared_ptr< T, Alloc >::shared_ptr(T *ptr, Alloc alloc)
    :m_next{this}
    ,m_object{ptr}
    ,m_alloc{alloc}
{

}

template< typename T, class Alloc >
shared_ptr< T, Alloc >::~shared_ptr()
{
    if (unique() && m_object) {
        m_object->~T();
        m_alloc.deallocate(m_object, 1);
    }
}

template< typename T, class Alloc >
shared_ptr< T, Alloc >::shared_ptr(shared_ptr &other)
    :m_next(&other)
    ,m_object(other.m_object)
    ,m_alloc(other.m_alloc)
{
    shared_ptr *prev = other.get_prev();
    prev->set_next(this);
}

template< typename T, class Alloc >
shared_ptr< T, Alloc >::shared_ptr(shared_ptr &&other)
    :m_next(other.m_next)
    ,m_object(other.m_object)
    ,m_alloc(other.m_alloc)
{
    other.m_next = &other;
    other.m_object = nullptr;
}

template< typename T, class Alloc >
shared_ptr< T, Alloc >& shared_ptr< T, Alloc >::operator=(shared_ptr &other)
{
    if (&other != this) {
        if (unique()) {
            // Release the resource
            if (m_object) {
                m_object->~T();
                m_alloc.deallocate(m_object, 1);
                m_object = nullptr;
            }
        } else {
            // Unlink me
            shared_ptr *prev = get_prev();
            shared_ptr *next = get_next();

            prev->set_next(next);
        }

        // Link me to the new company
        other.set_prev(this);
        this->set_next(&other);
        m_object = other.m_object;
        m_alloc  = other.m_alloc;
    }

    return *this;
}

template< typename T, class Alloc >
shared_ptr< T, Alloc >* shared_ptr< T, Alloc >::get_next() const
{
    return this->m_next;
}

template< typename T, class Alloc >
shared_ptr< T, Alloc >* shared_ptr< T, Alloc >::get_prev() const
{
    shared_ptr *cur = this->m_next;
    while (cur->m_next != this) {
        cur = cur->m_next;
    }

    return cur;
}

template< typename T, class Alloc >
void shared_ptr< T, Alloc >::set_next(shared_ptr *ptr)
{
    this->m_next = ptr;
}

template< typename T, class Alloc >
void shared_ptr< T, Alloc >::set_prev(shared_ptr *ptr)
{
    this->get_prev()->set_next(ptr);
}

template< typename T, class Alloc >
bool shared_ptr< T, Alloc >::unique() const
{
    return get_next() == this;
}

template< typename T, class Alloc >
T* shared_ptr< T, Alloc >::get() const
{
    return m_object;
}

//------------------------------------------------------------------------------

template< typename T, template< typename > class Alloc, class... Args >
shared_ptr< T, Alloc< T > > allocate_shared(const Alloc< T >& alloc, Args... args)
{
    auto allocator = alloc;
    T *ptr = allocator.allocate(1);
    assert(ptr); // TODO: add here valid check
    new (ptr) T{args ...};

    return shared_ptr< T, Alloc< T > >{ptr, alloc};
}


}

#endif // ECL_MEMORY_HPP_
