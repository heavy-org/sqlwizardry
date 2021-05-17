#ifndef INCLUDED_SQLWIZARDRY_V1_COLUMN_HPP
#define INCLUDED_SQLWIZARDRY_V1_COLUMN_HPP

#include <utility>
#include <type_traits>
#include <array>
#include <tuple>
#include <initializer_list>

#include <experimental/array>

#include <sqlwizardry/v1/predicate.hpp>
#include <sqlwizardry/v1/detail/traits.hpp>
#include <sqlwizardry/v1/detail/macro.hpp>


namespace sqlwizardry {
namespace column{
    struct PrimaryKey {};
    struct Required {};
    struct ForiegnKey {};
    struct Unique {};

    template <typename TYPE, typename... ARGS>
    struct Column : ARGS... {
        using type = std::decay_t<TYPE>;
    };

    template <typename Column>
    struct Ascending {
        Column column;
        Ascending(Column cols) : column{std::move(cols)} {}
    };

    template <typename Column>
    Ascending(Column&&) -> Ascending<std::decay_t<Column>>;

    template <typename Column>
    struct Descending {
        Column column;
        Descending(Column cols) : column{std::move(cols)} {}
    };

    template <typename Column>
    Descending(Column&&) -> Descending<std::decay_t<Column>>;
}




template <typename MODEL, typename COLUMN>
struct ColumnDefinition {
    using type = typename COLUMN::type;
    using model = MODEL;
    static constexpr std::string_view table_name = model::table_name;
    std::string_view name;

    SQLWIZARDRY_DEFINE_COMPOUND_TERMINAL_OPERATOR_MAPPING(Equals,==)
    SQLWIZARDRY_DEFINE_COMPOUND_TERMINAL_OPERATOR_MAPPING(NotEquals,!=)
    SQLWIZARDRY_DEFINE_COMPOUND_TERMINAL_OPERATOR_MAPPING(LessThan,<)
    SQLWIZARDRY_DEFINE_COMPOUND_TERMINAL_OPERATOR_MAPPING(LessThanOrEqual,<=)
    SQLWIZARDRY_DEFINE_COMPOUND_TERMINAL_OPERATOR_MAPPING(GreaterThan,>)
    SQLWIZARDRY_DEFINE_COMPOUND_TERMINAL_OPERATOR_MAPPING(GreaterThanOrEqual,>=)

    [[nodiscard]] constexpr auto in(std::initializer_list<type> items) const {
        return ColumnPredicate{In{*this, std::array{items}}};
    }

    template <typename STRING_T>
    [[nodiscard]] constexpr auto like(STRING_T&& str) const {
        static_assert(is_string<STRING_T>::value, "Input must be a string like type.");
        return ColumnPredicate{Like{*this, std::forward<STRING_T>(str)}};
    }

    template <typename T1, typename T2>
    [[nodiscard]] constexpr auto between(T1&& a, T2&& b) const {
        using common_type = std::common_type_t<std::decay_t<T1>, std::decay_t<T2>>;
        static_assert(std::is_convertible<common_type, type>::value, "Items must be convertible to column type.");
        return ColumnPredicate{Between{*this, std::pair<common_type, common_type>{std::forward<T1>(a),std::forward<T2>(b)}}};
    }

    template <typename... K>
    [[nodiscard]] constexpr auto in(K&& ...items) const {
        static_assert(std::is_convertible<std::common_type_t<K...>,type>::value, "Items must be convertible to column type.");
        return ColumnPredicate{In{*this, std::experimental::make_array(std::forward<K>(items)...)}};
    }
};  
}

#endif