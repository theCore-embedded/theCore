/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//!
//! \file
//! \brief Memory managment helpers: shared pointer.
//! \todo Implement unique poitner
//! \todo Detailed description
//!
#ifndef ECL_MEMORY_HPP_
#define ECL_MEMORY_HPP_


#include <ecl/assert.h>

#include <cstddef>
#include <utility>

namespace ecl
{

// Ctor in aux class left as default intentionally.
// aux class and its derived will be destroyed by calling `destroy()` method
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"

//!
//! \brief Base helper class, used internally by shared_ptr
//!
class aux
{
public:
    //! Constructs auxilary object
    aux()
        :m_cnt{0}, m_weak{0} { }

    //! Incremets reference counter and returns new value.
    size_t inc() { return ++m_cnt; }
    //! Decrements reference counter and returns new value.
    size_t dec() { return --m_cnt; }
    //! Returns reference counter.
    size_t ref() { return m_cnt;   }

    //! Incremets weak reference counter and returns new value.
    size_t weak_inc() { return ++m_weak; }
    //! Decrements weak reference counter and returns new value.
    size_t weak_dec() { return --m_weak; }
    //! Returns wek reference counter.
    size_t weak_ref() { return m_weak;   }

    //!
    //! \brief  Destroys aux and deallocates memory if both shared and weak
    //!         counter is zero.
    //!
    //! Destructor of the managed object must not be called here.
    //! _Must_ be called to destroy whole object.
    virtual void destroy() = 0;

protected:
    size_t m_cnt;       //!< Reference counter.
    size_t m_weak;      //!< Weak reference counter.
};

#pragma GCC diagnostic pop

//------------------------------------------------------------------------------

//!
//! \brief Classical shared pointer.
//! \todo Mention about manadatory allocator for creating shared pointer
//!
template< typename T >
class shared_ptr
{
    //! \brief Allocates a shared pointer.
    //! \tparam U Is a T or is a subclass of T.
    template< typename U, class Alloc, class... Args >
    friend shared_ptr< U > allocate_shared(const Alloc& alloc, Args... args);

    //! \brief Related shared pointer type.
    //! \tparam U Is a subclass of T.
    template< typename U >
    friend class shared_ptr;

    //! \brief Weak reference.
    //! \tparam U Is a T or a subclass of T.
    template< typename U >
    friend class weak_ptr;

public:
    //! Constructs default shared pointer with no managed object.
    shared_ptr();
    //! Destroys shared pointer and if counter
    virtual ~shared_ptr();

    //! Initializes shared pointer with nullpointer
    shared_ptr(std::nullptr_t nullp);

    //!
    //! \brief   Constrcuts from existing pointer and inherit its ownership.
    //! \details If other pointer manages no object, this manages no object too.
    //! \param[in] other Valid shared pointer.
    //!
    shared_ptr(const shared_ptr &other);

    //!
    //! \brief   Moves other shared pointer and inherit ownership.
    //! \details This will not increase refernce counter, obviously.
    //! \param[in] other Valid shared pointer to move. After moving pointer will
    //!                  no longer hold any reference and will return to the
    //!                  default state.
    //!
    shared_ptr(shared_ptr &&other);

    //!
    //! \brief   Shares ownership of the object with a pointer provided.
    //! \details This shared pointer will release ownership of the previously
    //!          managed object if any. In case other pointer owns an
    //!          object then new ownership will be applied to this pointer
    //!          and it will be shared with other pointer.
    //! \param[in] other Valid shared pointer.
    //! \return This shared object.
    //!
    shared_ptr& operator=(const shared_ptr &other);

    //!
    //! \brief Moves ownership from the pointer provided.
    //! \details  This shared pointer will release ownership of the previously
    //!           managed object if any. Other pointer will no longer own
    //!           any object.
    //! \param[in] other Valid shared pointer.
    //! \return This shared object.
    //!
    shared_ptr& operator=(shared_ptr &&other);

    //! Returns true if this shared pointer manages the object.
    explicit operator bool() const;

    //!
    //! \copydoc shared_ptr(const shared_ptr &other)
    //! \tparam U Is a subclass of T
    //!
    template< typename U >
    shared_ptr(shared_ptr< U > &other);

    //!
    //! \brief Shares ownership with a shared pointer of the derived type.
    //! \tparam U Is a subclass of T
    //!
    template< typename U >
    shared_ptr& operator=(shared_ptr< U > &other);

    //! Returns true if this pointer holds the last remaining node
    bool unique() const;

    //! Returs a value itself
    T* get() const;

    //! Common smart pointer overload.
    T& operator *();
    //! Common smart pointer overload.
    const T& operator*() const;
    //! Common smart pointer overload.
    T* operator ->();
    //! Common smart pointer overload.
    const T* operator ->() const;

private:

// Ctor in aux class left as default intentionally.
// aux class and its derived will be destroyed by calling `destroy()` method
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"

    //! Type-erased helper class
    template< class Alloc, class... Args >
    class aux_alloc : public aux
    {
    public:
        aux_alloc(const Alloc &a, Args... args)
            :aux{}
            ,m_object(args...)
            ,m_alloc(a)
        { }

        //! \copydoc aux::destroy
        virtual void destroy() override
        {
            // Make sure there is no other references
            ecl_assert(!m_cnt);

            // No more weak references so this object may be deleted
            if (!m_weak) {
                auto allocator = m_alloc.template rebind< aux_alloc >();
                allocator.deallocate(this, 1);
            }
        }

        // Cannot be deleted by calling dtor
        ~aux_alloc() = delete;

        T        m_object; //! The managed object.

    private:
        Alloc    m_alloc;  //! The allocator used to allocated this object.
    };

#pragma GCC diagnostic pop

    //! Releases the ownership.
    //! \todo: describe post and pre conditions
    void release();

    aux  *m_aux; //! Helper object.
    T    *m_obj; //! Object itself.
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
    release();
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
    if (m_aux) {
        m_aux->inc();
    }
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
        release();

        m_aux = other.m_aux;
        m_obj = other.m_obj;

        // Empty shared pointer assignment can be allowed, too.
        if (m_aux) {
            m_aux->inc();
        }
    }

    return *this;
}

template< typename T >
shared_ptr< T >& shared_ptr< T >::operator=(shared_ptr &&other)
{
    if (&other != this) {
        release();

        std::swap(m_aux, other.m_aux);
        std::swap(m_obj, other.m_obj);
    }

    return *this;
}

template< typename T >
template< typename U >
shared_ptr< T >::shared_ptr(shared_ptr< U > &other)
    :m_aux(other.m_aux)
    ,m_obj(other.m_obj)
{
    if (m_aux) {
        m_aux->inc();
    }
    // Else do nothing. Copy constructing from empty shared pointer is allowed.
}

template< typename T >
template< typename U >
shared_ptr< T >& shared_ptr< T >::operator=(shared_ptr< U > &other)
{
    release();

    m_aux = other.m_aux;
    m_obj = other.m_obj;

    // Empty shared pointer assigment can be allowed, too.
    if (m_aux) {
        m_aux->inc();
    }

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
    ecl_assert(obj);
    return *obj;
}

template< typename T >
const T& shared_ptr< T >::operator *() const
{
    auto obj = get();
    ecl_assert(obj);
    return *obj;
}

template< typename T >
T* shared_ptr< T >::operator ->()
{
    auto obj = get();
    ecl_assert(obj);
    return obj;
}

template< typename T >
const T* shared_ptr< T >::operator ->() const
{
    auto obj = get();
    ecl_assert(obj);
    return obj;
}


template< typename T >
shared_ptr< T >::operator bool() const
{
    return !!get();
}

//------------------------------------------------------------------------------

template< typename T >
void shared_ptr< T >::release()
{
    if (m_aux) {
        if (unique()) {
            // Release the resource
            m_obj->~T();
            // Object of T can contain weak reference of the same resource.
            // Thus it is prohibited to decrease counter before
            // object destruction.
            m_aux->dec();
            m_aux->destroy();
        } else {
            m_aux->dec();
        }

        m_aux = nullptr;
        m_obj = nullptr;
    }
}

//------------------------------------------------------------------------------

template< typename T, class Alloc, class... Args >
shared_ptr< T > allocate_shared(const Alloc &alloc, Args... args)
{
    using Aux = typename shared_ptr< T >:: template aux_alloc< Alloc, Args... >;

    // Rebind an allocator to use with the auxilarity object
    auto allocator = alloc.template rebind< Aux >();
    auto *ptr = allocator.allocate(1);
    ecl_assert(ptr); // TODO: add here valid check

    // Init auxiliary object
    new (ptr) Aux{alloc, args...};

    // Construct a pointer
    shared_ptr< T > shared;
    shared.m_aux = ptr;
    // Now it owns the resourse
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

// Comparison routines

template< typename T >
bool operator ==(const shared_ptr< T > &shr, std::nullptr_t nullp)
{
    // gcc version 6.1.1 20160501 produces warning if parenthesis aren't used.
    return shr.get() == (nullp);
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
    ecl_assert(m_aux);
    ecl_assert(m_obj);
    // Must be called only if there are no more strong references
    ecl_assert(!m_aux->ref());

    if (!m_aux->weak_dec()) {
        m_aux->destroy();
    }

    m_aux = nullptr;
    m_obj = nullptr;
}

}

#endif // ECL_MEMORY_HPP_
