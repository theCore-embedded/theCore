//! \file
//! \brief General the Core utilities library.
#ifndef ECL_UTILS_HPP_
#define ECL_UTILS_HPP_

#include <tuple>
#include <utility>

namespace ecl
{

// Proudly taken from stackoverflow :)
// http://stackoverflow.com/questions/1198260/iterate-over-tuple

template< std::size_t I = 0, typename FuncT, typename... Tp >
inline typename std::enable_if< I == sizeof...(Tp), void >::type
for_each(std::tuple<Tp...> &, FuncT)
{ }

template<std::size_t I = 0, typename FuncT, typename... Tp >
inline typename std::enable_if< I < sizeof...(Tp), void >::type
for_each(std::tuple<Tp...>& t, FuncT f)
{
    f(std::get<I>(t));
    for_each<I + 1, FuncT, Tp...>(t, f);
}

//! Obtains offset of the given member within given type.
//! \details Give a star for this!
//! Taken from https://gist.github.com/graphitemaster/494f21190bb2c63c5516
//! \todo move this to right module
template< typename T1, typename T2 >
inline typename std::enable_if< std::is_literal_type< T2 >::value, size_t >::type
constexpr offset_of(T1 T2::*member) {
    constexpr T2 object {};
    return size_t(&(object.*member)) - size_t(&object);
}

//! Obtains offset of the given member within given type.
//! \todo move this to right module
template< typename T1, typename T2 >
inline size_t constexpr offset_of(T1 T2::*member) {
    return reinterpret_cast< size_t >(&(((T2*)nullptr)->*member));
}

}

#endif
