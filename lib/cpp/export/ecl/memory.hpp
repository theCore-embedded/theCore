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
        :m_cnt{0}, m_weak{0} { }

    // Increments/decrements/gets the reference counter
    size_t inc() { return ++m_cnt; }
    size_t dec() { return --m_cnt; }
    size_t ref() { return m_cnt;   }

    // Reference counting for weak references
    size_t weak_inc() { return ++m_weak; }
    size_t weak_dec() { return --m_weak; }
    size_t weak_ref() { return m_weak;   }

    // Destroys aux and deallocates memory if both shared and weak counter is zero.
    // Destructor of the managed object must not be called here.
    // _Must_ be called to destroy whole object.
    virtual void destroy() = 0;

protected:
    size_t m_cnt;
    size_t m_weak;
};

//------------------------------------------------------------------------------

template< typename T >
class shared_ptr
{
    template< typename U, class Alloc, class... Args >
    friend shared_ptr< U > allocate_shared(const Alloc& alloc, Args... args);

    template< typename U >
    friend class shared_ptr;

    template< typename U >
    friend class weak_ptr;

    template< typename U, class Alloc >
    friend struct allocation_size_info;

public:
    shared_ptr();
    virtual ~shared_ptr();

    shared_ptr(std::nullptr_t nullp);
    shared_ptr(const shared_ptr &other);
    shared_ptr(shared_ptr &&other);
    shared_ptr& operator=(const shared_ptr &other);
    explicit operator bool() const;

    // Constructs/assigns pointer from dependent type.
    // T and U must be in relation, such that
    // U is subclass of T.
    template< typename U >
    shared_ptr(shared_ptr< U > &other);
    template< typename U >
    shared_ptr& operator=(shared_ptr< U > &other);

    // Returns true if this pointer holds the last remaining node
    bool unique() const;
    // Returns true if no weak pointers exists that points to the same object
    bool weak_unique() const;

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
            ,m_object(args...)
            ,m_alloc(a)
        { }

        virtual void destroy() override
        {
            // Make sure there is no other references
            assert(!m_cnt);

            // No more weak references so this object may be deleted
            if (!m_weak) {
                auto allocator = m_alloc.template rebind< aux_alloc >();
                allocator.deallocate(this, 1);
            }
        }

        // Cannot be deleted by calling dtor
        ~aux_alloc() = delete;

        // An object itself
        T        m_object;

    private:
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
            // Object of T can contain weak reference of the same resourse.
            // Thus it is prohibited to decrease counter before
            // object desctruction.
            m_obj->~T();
            // Now we can complete.
            m_aux->dec();
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
        if (m_aux) {
            if (unique()) {
                // Release the resource
                m_obj->~T();
                // Object of T can contain weak reference of the same resourse.
                // Thus it is prohibited to decrease counter before
                // object desctruction.
                m_aux->dec();
                m_aux->destroy();
            } else {
                m_aux->dec();
            }
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
    :m_aux(other.m_aux)
    ,m_obj(other.m_obj)
{
    if (m_aux)
        m_aux->inc();
    // Else do nothing. Copy constructing from empty shared pointer is allowed.
}

template< typename T >
template< typename U >
shared_ptr< T >& shared_ptr< T >::operator=(shared_ptr< U > &other)
{
    if (m_aux) {
        if (unique()) {
            // Release the resource
            m_obj->~T();
            // Object of T can contain weak reference of the same resourse.
            // Thus it is prohibited to decrease counter before
            // object desctruction.
            m_aux->dec();
            m_aux->destroy();
        } else {
            m_aux->dec();
        }
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
    return m_aux ? m_aux->ref() == 1 : true;
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


template< typename T >
shared_ptr< T >::operator bool() const
{
    return !!get();
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
    // Now it is owning the resourse
    shared.m_aux->inc();
    shared.m_obj = &ptr->m_object;

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

template< typename T >
bool operator !(const shared_ptr< T > &shr)
{
    return !shr.get();
}


//------------------------------------------------------------------------------

// Weak pointer
template< typename T >
class weak_ptr
{
public:
    constexpr weak_ptr();
    virtual ~weak_ptr();

    weak_ptr(const weak_ptr &other);
    weak_ptr(weak_ptr &&other);
    weak_ptr(const shared_ptr< T > &other);

    weak_ptr& operator =(weak_ptr other);
    weak_ptr& operator =(weak_ptr &&other) = default;
    weak_ptr& operator =(const shared_ptr< T > &other);

    void swap(weak_ptr &other);

    shared_ptr< T > lock() const;
    bool expired() const;
    void reset() const;

private:
    // As for now, this pointer will be valid
    // until all weak pointers will be destroyed even if managed
    // object no longer valid.
    mutable aux *m_aux;
    mutable T *m_obj;
};


template< typename T >
constexpr weak_ptr< T >::weak_ptr()
    :m_aux{nullptr}
    ,m_obj{nullptr}
{
}

template< typename T >
weak_ptr< T >::~weak_ptr()
{
    if (!expired()) {
        m_aux->weak_dec();
    }
    // Do nothing here, already expired...
}

template< typename T >
weak_ptr< T >::weak_ptr(const weak_ptr &other)
    :weak_ptr{}
{
    if (!other.expired()) {
        m_obj = other.m_obj;
        m_aux = other.m_aux;
        m_aux->weak_inc();
    }
}

template< typename T >
weak_ptr< T >::weak_ptr(weak_ptr &&other)
    :weak_ptr{}
{
    swap(other);
}

template< typename T >
weak_ptr< T >::weak_ptr(const shared_ptr<T> &other)
    :m_aux{other.m_aux}
    ,m_obj{other.m_obj}
{
    if (!expired()) {
        m_aux->weak_inc();
    }
}

template< typename T >
weak_ptr< T >& weak_ptr< T >::operator =(weak_ptr< T > other)
{
    swap(other);
    return *this;
}

template< typename T >
weak_ptr< T >& weak_ptr< T >::operator =(const shared_ptr< T > &other)
{
    if (!expired()) {
        m_aux->weak_dec();
        m_aux = nullptr;
        m_obj = nullptr;
    }

    m_aux = other.m_aux;
    m_obj = other.m_obj;

    if (!expired())
        m_aux->weak_inc();

    return *this;
}

template< typename T >
void weak_ptr< T >::swap(weak_ptr< T > &other)
{
    std::swap(other.m_aux, this->m_aux);
    std::swap(other.m_obj, this->m_obj);
}

template< typename T >
shared_ptr< T > weak_ptr< T >::lock() const
{
    if (expired()) {
        return nullptr;
    } else {
        // Weak counter already incremented
        m_aux->inc();

        shared_ptr< T > ptr;
        ptr.m_aux = m_aux;
        ptr.m_obj = m_obj;
        return ptr;
    }
}

template< typename T >
bool weak_ptr< T >::expired() const
{
    if (!m_aux) {
        return true;
    }

    // No more strong references
    if (!m_aux->ref()) {
        reset();
        return true;
    }

    return false;
}

template< typename T >
void weak_ptr< T >::reset() const
{
    // Valid object is required
    assert(m_aux);
    assert(m_obj);
    // Must be called only if there are no more strong references
    assert(!m_aux->ref());

    if (!m_aux->weak_dec()) {
        m_aux->destroy();
    }

    m_aux = nullptr;
    m_obj = nullptr;
}

}

#endif // ECL_MEMORY_HPP_
