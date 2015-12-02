#ifndef ECL_MEMORY_HPP_
#define ECL_MEMORY_HPP_

#include <cstddef>
#include <ecl/assert.hpp>

namespace ecl
{

// The base helper class
class aux
{
public:
    aux()
        :m_cnt{0} { }

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

template< typename T >
class shared_ptr
{
    template< typename U, class Alloc, class... Args >
    friend shared_ptr< U > allocate_shared(const Alloc& alloc, Args... args);

    template< typename U >
    friend class shared_ptr;

    template< typename U, class Alloc >
    friend struct allocation_size_info;

public:
    shared_ptr();
    ~shared_ptr();

    shared_ptr(std::nullptr_t nullp);
	shared_ptr(const shared_ptr &other);
    shared_ptr(shared_ptr &&other);
	shared_ptr& operator=(const shared_ptr &other);

    // Constructs/assigns pointer from dependent type.
    // T and U must be in relation, such that
    // U is subclass of T.
    template< typename U >
    shared_ptr(shared_ptr< U > &other);
    template< typename U >
    shared_ptr& operator=(shared_ptr< U > &other);

    // Returns true if this pointer holds the last remaining node
    bool unique() const;

    // Returs a value itself
    T* get() const;

    // Common smart pointer overloads
    T& operator *();
    const T& operator*() const;
    T* operator ->();
    const T* operator ->() const;

private:
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

        // Cannot be deleted by calling dtor
        ~aux_alloc() = delete;

    private:
        // An object itself
        T        m_object;
        Alloc    m_alloc;
    };

    // Helper object
    aux  *m_aux;
    // Object itself
    T    *m_obj;
};

//------------------------------------------------------------------------------

template< typename T >
shared_ptr< T >::shared_ptr()
    :m_aux{nullptr}
    ,m_obj{nullptr}
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
shared_ptr< T >::shared_ptr(std::nullptr_t nullp)
    :m_aux{nullp}
    ,m_obj{nullp}
{

}

template< typename T >
shared_ptr< T >::shared_ptr(const shared_ptr &other)
    :m_aux{other.m_aux}
    ,m_obj{other.m_obj}
{

    if (m_aux)
        m_aux->inc();
    // Else do nothing. Copy constructing from empty shared pointer is allowed.
}

template< typename T >
shared_ptr< T >::shared_ptr(shared_ptr &&other)
    :m_aux{other.m_aux}
    ,m_obj{other.m_obj}
{
    other.m_aux = nullptr;
    other.m_obj = nullptr;
}

template< typename T >
shared_ptr< T >& shared_ptr< T >::operator=(const shared_ptr &other)
{
    if (&other != this) {
        if (unique()) {
            // Release the resource
            if (m_aux) {
                m_aux->destroy();
                m_aux = nullptr;
                m_obj = nullptr;
            }
        } else {
            // If it is not single shrared pointer,
            // then auxilary object is guaranteed to exist.
            m_aux->dec();
        }

        m_aux = other.m_aux;
        m_obj = other.m_obj;

        // Empty shared pointer assigment can be allowed, too.
        if (m_aux)
            m_aux->inc();
    }

    return *this;
}

template< typename T >
template< typename U >
shared_ptr< T >::shared_ptr(shared_ptr< U > &other)
    :m_aux{other.m_aux}
    ,m_obj{other.m_obj}
{
    (void) other;

    if (m_aux)
        m_aux->inc();
    // Else do nothing. Copy constructing from empty shared pointer is allowed.
}

template< typename T >
template< typename U >
shared_ptr< T >& shared_ptr< T >::operator=(shared_ptr< U > &other)
{
    if (unique()) {
        // Release the resource
        if (m_aux) {
            m_aux->destroy();
            m_aux = nullptr;
            m_obj = nullptr;
        }
    } else {
        // If it is not single shrared pointer,
        // then auxilary object is guaranteed to exist.
        m_aux->dec();
    }

    m_aux = other.m_aux;
    m_obj = other.m_obj;

    // Empty shared pointer assigment can be allowed, too.
    if (m_aux)
        m_aux->inc();

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
    return m_obj;
}

template< typename T >
T& shared_ptr< T >::operator *()
{
    auto obj = get();
    assert(obj);
    return *obj;
}

template< typename T >
const T& shared_ptr< T >::operator *() const
{
    auto obj = get();
    assert(obj);
    return *obj;
}

template< typename T >
T* shared_ptr< T >::operator ->()
{
    auto obj = get();
    assert(obj);
    return obj;
}

template< typename T >
const T* shared_ptr< T >::operator ->() const
{
    auto obj = get();
    assert(obj);
    return obj;
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

// Provides estimation of how much memory will be allocated
// by shared pointer
template< typename T, class Alloc >
struct shared_allocation_size
{
    using aux_type = typename shared_ptr< T >::template aux_alloc< T, Alloc >;
    static constexpr size_t value = sizeof(aux_type);
};

//------------------------------------------------------------------------------

// Comparsion routines

template< typename T >
bool operator ==(const shared_ptr< T > &shr, std::nullptr_t nullp)
{
    return shr.get() == nullp;
}

template< typename T >
bool operator !=(const shared_ptr< T > &shr, std::nullptr_t nullp)
{
    return shr.get() == nullp;
}

template< typename T >
bool operator ==(const shared_ptr< T > &shr1, const shared_ptr< T > &shr2)
{
    return shr1.get() == shr2.get();
}

template< typename T >
bool operator !=(const shared_ptr< T > &shr1, const shared_ptr< T > &shr2)
{
    return shr1.get() != shr2.get();
}

//------------------------------------------------------------------------------

}

#endif // ECL_MEMORY_HPP_
