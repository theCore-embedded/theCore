/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief General the Core utilities library.
#ifndef LIB_ECL_UTILS_HPP_
#define LIB_ECL_UTILS_HPP_

#include <tuple>
#include <utility>

namespace ecl
{

//------------------------------------------------------------------------------

// Special credit to RostakaGmfun for providing cool way to iterate tuple.

template <class FuncT, class TupleT, size_t ...Is>
void for_each_impl(TupleT &tuple, FuncT func, std::index_sequence<Is...>)
{
    (void)std::initializer_list<int>{(func(std::get<Is>(tuple)), 0)...};
}

template<class FuncT, class ...Tp>
void for_each(std::tuple<Tp...> &tuple, FuncT func)
{
    for_each_impl(tuple, func, std::index_sequence_for<Tp...>{});
}

//! For-each for types only.
//! \tparam Callable Type with operator() overloaded
//! \tparam Tp Types for with Callable will be called
template<template<typename> class Callable, class ...Tp>
void for_each()
{
    [](...){}((Callable<Tp>{}(), 0) ... );
}

//------------------------------------------------------------------------------

//! Obtains offset of the given member within given type.
template<typename T1, typename T2>
inline constexpr size_t offset_of(T1 T2::*member) {
    std::aligned_storage<sizeof(T2), alignof(T2)> object;
    T2 *ptr = reinterpret_cast<T2 *>(&object);
    return reinterpret_cast<uint8_t *>(&(ptr->*member)) - reinterpret_cast<uint8_t *>(ptr);
}

//------------------------------------------------------------------------------

//! Safe storage wrapper to avoid static initialization fiasco
//! \tparam T non-POD type to protect against implicit constructor call during static initialization
template<class T>
struct safe_storage
{
    //! Initializes storage.
    //! \tparam Args Types of the constructor args.
    //! \param  args T's ctor arguments.
    //! \pre  Storage not yet initialized, i.e. ctor wasn't called yet
    //!       or storage was deinitialized earlier.
    //! \post Storage initialized and object is ready to work.
    //! \note Violating pre-conditions leads to undefined behaviour.
    template<class ...Args>
    void init(Args&& ...args)
    {
        new (&m_stor) T{std::forward<Args>(args)...};
    }

    //! Deinitializes storage.
    //! \pre  Storage is initialized.
    //! \post Storage is deinitialized and stored object is destructed.
    //! \note Violating pre-conditions leads to undefined behaviour.
    void deinit()
    {
        get().~T();
    }

    //! Gets reference to the stored object.
    //! \pre Storage initialized, i.e. init() was called once.
    T& get()
    {
        return reinterpret_cast<T&>(m_stor);
    }

    //! Gets const reference to the stored object.
    //! \pre Storage initialized, i.e. init() was called once.
    const T& get() const
    {
        return reinterpret_cast<T&>(m_stor);
    }

    //! Storage for the T object.
    std::aligned_storage_t<sizeof(T), alignof(T)> m_stor;
};

// Redundant check to avoid potential bugs when changes will be applied to
// this struct.
// Making this type trival will make sure that it will be initialized
static_assert(std::is_trivial<safe_storage<int>>::value,
    "storage struct must be trivial");

//------------------------------------------------------------------------------

//! Extracts value from enumerator element of given type.
//! \tparam Enum Enumerator type.
//! \param[in] val Value of the enumerator type.
//! \return Integer representation of the enum value.
template<typename Enum>
constexpr auto extract_value(Enum val)
{
    static_assert(std::is_enum<Enum>::value, "Given type is not enumeration.");

    return static_cast<std::underlying_type_t<Enum>>(val);
}

//------------------------------------------------------------------------------

//! Calculates string length in compile time, if possible.
template<int N>
constexpr auto strlen_constexpr(const char (&str)[N])
{
    size_t len = 0;
    while (str[len++]) { }
    return len - 1;
}

//! Calculates string length in compile time, if possible.
//! \details Overload for consuming pointers.
constexpr auto strlen_constexpr(const char *str)
{
    size_t len = 0;
    while (str[len++]) { }
    return len - 1;
}

//! Returns an index to the first occurrence of the character in the string.
//! \details Usable in compile-time
//! \return Index of a character in string where given char is located,
//!         -1 otherwise.
constexpr int strchri_constexpr(const char *str, char ch)
{
    int ret = -1;
    int i = 0;
    while (str[i]) { if (str[i] == ch) { ret = i; break; }; ++i; }
    return ret;
}

//------------------------------------------------------------------------------

//! Exploded string produces unique type for every unique string
template<char... c>
struct exploded_string
{
    //! Exploded string static array
    static const char fl[sizeof ...(c)];
};

template<char ...c>
const char exploded_string<c...>::fl[sizeof ...(c)] = { c... };

//! Explodes substring into `exploded_string` type.
//! \tparam S String advertiser with str() method that will return valid
//!           constexpr string.
//! \tparam B Index of begin of substring to be exploded.
//! \tparam L Length of substring to be exploded.
template <typename S, size_t B, size_t L, char... c>
struct explode_chunk_impl
{
    using result =
    typename explode_chunk_impl<S, B, L - 1, S::str()[B + L - 1], c...>::result;
};

//! Explodes part of the string into `exploded_string` type.
//! \tparam S String advertiser with str() method that will return valid
//!           constexpr string.
//! \tparam B Index of begin of substring to be exploded.
template <typename S, size_t B, char... c >
struct explode_chunk_impl<S, B, 0, c...>
{
    using result = exploded_string<c...>;
};

//! Syntactical sugar for exploding substring.
//! \tparam S String advertiser with str() method that will return valid
//!           constexpr string.
//! \tparam B Index of begin of substring to be exploded.
//! \tparam L Length of substring to be exploded.
template <typename S, size_t B, size_t L>
using explode_chunk = typename explode_chunk_impl<S, B, L>::result;

} // namespace ecl

#endif // LIB_ECL_UTILS_HPP_
