#ifndef ECL_MEMORY_HPP_
#define ECL_MEMORY_HPP_

#include <cstddef>

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
    // The base helper class
    class aux
    {
    public:
        aux()
            :m_cnt{0} { }

        // Gets object
        virtual T* get() = 0;
        // Increments/decrements/gets the reference counter
        size_t inc() { return ++m_cnt; }
        size_t dec() { return --m_cnt; }
        size_t ref() { return m_cnt;   }

        // Destroys payload and aux itself.
        // _Must_ be called to destroy whole object
        virtual void destroy() = 0;
    private:
        size_t m_cnt;
    };

    // Type-erased helper class
    template< class Alloc, class... Args >
    class aux_alloc : public aux
    {
    public:
        aux_alloc(const Alloc &a, Args... args)
            :aux{}
            ,m_object{args...}
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

    // Helper object
    aux        *m_aux;
};

//------------------------------------------------------------------------------

template< typename T >
shared_ptr< T >::shared_ptr()
    :m_aux{nullptr}
{

}

template< typename T >
shared_ptr< T >::~shared_ptr()
{
    if (m_aux) {
        if (unique()) {
            m_aux->destroy();
        } else {
            m_aux->dec();
        }
    }
}

template< typename T >
shared_ptr< T >::shared_ptr(shared_ptr &other)
    :m_aux{other.m_aux}
{

    if (m_aux)
        m_aux->inc();
    // Else do nothing. Copy constructing from empty shared pointer is allowed.
}

template< typename T >
shared_ptr< T >::shared_ptr(shared_ptr &&other)
    :m_aux{other.m_aux}
{
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
            // If it is not single shrared pointer,
            // then auxilary object is guaranteed to exist.
            m_aux->dec();
        }

        m_aux = other.m_aux;

        // Empty shared pointer assigment can be allowed, too.
        if (m_aux)
            m_aux->inc();
    }

    return *this;
}

template< typename T >
bool shared_ptr< T >::unique() const
{
    return m_aux ? m_aux->ref() == 0 : 0;
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
