#ifndef ECL_MEMORY_HPP_
#define ECL_MEMORY_HPP_

namespace ecl
{

template< typename T >
class shared_ptr;

template< typename T, class Alloc, class... Args >
shared_ptr< T > allocate_shared(const Alloc &alloc, Args... args);

template< typename T >
class shared_ptr
{
    template< typename U, class Alloc, class... Args >
    friend shared_ptr< U > allocate_shared(const Alloc& alloc, Args... args);

public:
    shared_ptr();
    ~shared_ptr();

    shared_ptr(shared_ptr &other);
    shared_ptr(shared_ptr &&other);
    shared_ptr& operator=(shared_ptr &other);

    // Returns true if this pointer holds the last remaining node
    bool unique() const;

    // Returs a value itself
    T* get() const;

private:
    struct aux
    {
        // Gets object
        virtual T* get() = 0;
        // Destroys payload and aux itself. _Must_ be called to destroy
        virtual void destroy() = 0;
    };

    // Helper object
    template< class Alloc, class... Args >
    class aux_alloc : public aux
    {
    public:
        aux_alloc(const Alloc &a, Args... args)
            :m_object{args...}
            ,m_alloc{a}
        { }

        virtual void destroy() override
        {
            m_object.~T();
            auto allocator = m_alloc.template rebind< aux_alloc >();
            allocator.deallocate(this, 1);
        }

        T* get() override
        {
            return &m_object;
        }

        // Cannot be deleted by calling dtor
        ~aux_alloc() = delete;

    private:
        // An object itself
        T        m_object;
        Alloc    m_alloc;
    };

    // Pointers are stored in the single-linked circular list
    shared_ptr *m_next;
    // Helper object
    aux        *m_aux;

    // List traverse helpers
    shared_ptr* get_next() const;
    shared_ptr* get_prev() const;

    void set_next(shared_ptr *ptr);
    void set_prev(shared_ptr *ptr);
};


//------------------------------------------------------------------------------

template< typename T >
shared_ptr< T >::shared_ptr()
    :m_next{this}
    ,m_aux{nullptr}
{

}

template< typename T >
shared_ptr< T >::~shared_ptr()
{
    if (unique() && m_aux) {
        m_aux->destroy();
    } else {
        // Unlink me
        //ecl::cout << "unlink in desc" << ecl::endl;
        shared_ptr *prev = get_prev();
        shared_ptr *next = get_next();

        prev->set_next(next);
    }
}

template< typename T >
shared_ptr< T >::shared_ptr(shared_ptr &other)
    :m_next{&other}
    ,m_aux{other.m_aux}
{
    shared_ptr *prev = other.get_prev();
    prev->set_next(this);
}

template< typename T >
shared_ptr< T >::shared_ptr(shared_ptr &&other)
    :m_next{other.m_next}
    ,m_aux{other.m_aux}
{
    other.m_next = &other;
    other.m_aux = nullptr;
}

template< typename T >
shared_ptr< T >& shared_ptr< T >::operator=(shared_ptr &other)
{
    if (&other != this) {
        if (unique()) {
            // Release the resource
            if (m_aux) {
                m_aux->destroy();
                m_aux = nullptr;
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
        m_aux = other.m_aux;
    }

    return *this;
}

template< typename T >
shared_ptr< T >* shared_ptr< T >::get_next() const
{
    return this->m_next;
}

template< typename T >
shared_ptr< T >* shared_ptr< T >::get_prev() const
{
    shared_ptr *cur = this->m_next;
    while (cur->m_next != this) {
        cur = cur->m_next;
    }

    return cur;
}

template< typename T >
void shared_ptr< T >::set_next(shared_ptr *ptr)
{
    this->m_next = ptr;
}

template< typename T >
void shared_ptr< T >::set_prev(shared_ptr *ptr)
{
    this->get_prev()->set_next(ptr);
}

template< typename T >
bool shared_ptr< T >::unique() const
{
    return get_next() == this;
}

template< typename T >
T* shared_ptr< T >::get() const
{
    return m_aux ? m_aux->get() : nullptr;
}

//------------------------------------------------------------------------------

template< typename T, class Alloc, class... Args >
shared_ptr< T > allocate_shared(const Alloc &alloc, Args... args)
{
    using Aux = typename shared_ptr< T >:: template aux_alloc< Alloc, Args... >;

    // Rebind an allocator to use with the auxilarity object
    auto allocator = alloc.template rebind< Aux >();
    auto *ptr = allocator.allocate(1);
    assert(ptr); // TODO: add here valid check

    // Init auxilary object
    new (ptr) Aux{alloc, args...};

    // Construct a pointer
    shared_ptr< T > shared;
    shared.m_aux = ptr;

    return shared;
}


}

#endif // ECL_MEMORY_HPP_
