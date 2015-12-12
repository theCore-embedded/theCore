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

}

#endif
