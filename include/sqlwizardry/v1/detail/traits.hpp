#ifndef INCLUDED_SQLWIZARDRY_V1_DETAIL_TRAITS_HPP
#define INCLUDED_SQLWIZARDRY_V1_DETAIL_TRAITS_HPP

#include <type_traits>
#include <string_view>
#include <string>


namespace sqlwizardry {
namespace detail {
    template <typename T>
    struct is_string : std::false_type {};
    template <>
    struct is_string<std::string> : std::true_type {};
    template <>
    struct is_string<std::string_view> : std::true_type {};
    template <>
    struct is_string<const char *> : std::true_type {};
    template <std::size_t N>
    struct is_string<const char(&)[N]> : std::true_type {};
    template <>
    struct is_string<char *> : std::true_type {};
    template <std::size_t N>
    struct is_string<char(&)[N]> : std::true_type {};
    template <std::size_t N>
    struct is_string<char[N]> : std::true_type {};
}

//Forward declarations
namespace column{
template<typename TYPE, typename ...T>
struct Column;
template<typename T>
struct Ascending;
template<typename T>
struct Descending;
}
template <typename T>
struct is_column : std::false_type {};

template <typename T, typename... REST>
struct is_column<sqlwizardry::column::Column<T, REST...>> : std::true_type {};

struct empty;

template <typename T>
using is_string = detail::is_string<std::decay_t<T>>;
template <typename T>
constexpr auto is_string_v = detail::is_string<std::decay_t<T>>::value;

template <typename... T>
struct contains;

template <typename T>
struct contains<T> : std::false_type {};

template <typename SEARCH, typename LAST>
struct contains<SEARCH, LAST> : std::is_same<SEARCH, LAST> {};

template <typename SEARCH, typename FIRST, typename... REST>
struct contains<SEARCH, FIRST, REST...> : std::conditional_t<std::is_same_v<SEARCH, FIRST>, std::true_type, typename contains<SEARCH, REST...>::type> {};

template <typename... T>
constexpr auto contains_v = contains<T...>::value;


template <typename T>
struct is_ordering : std::false_type {};
template <typename Columns>
struct is_ordering<column::Ascending<Columns>> : std::true_type {};
template <typename Columns>
struct is_ordering<column::Descending<Columns>> : std::true_type {};
template <typename T>
constexpr auto is_ordering_v = is_ordering<T>::value;
template <typename T>
struct is_ascending : std::false_type {};
template <typename Columns>
struct is_ascending<column::Ascending<Columns>> : std::true_type {};
template <typename T>
constexpr auto is_ascending_v = is_ascending<T>::value;
template <typename T>
struct is_descending : std::false_type {};
template <typename Columns>
struct is_descending<column::Descending<Columns>> : std::true_type {};
template <typename T>
constexpr auto is_descending_v = is_descending<T>::value;

template <typename T>
struct is_empty : std::false_type{};
template <>
struct is_empty<empty> : std::true_type{};
template <>
struct is_empty<const empty> : std::true_type{};
template <>
struct is_empty<empty&> : std::true_type{};
template <>
struct is_empty<const empty&> : std::true_type{};

template <typename T>
constexpr auto is_empty_v = is_empty<T>::value;

}

#endif