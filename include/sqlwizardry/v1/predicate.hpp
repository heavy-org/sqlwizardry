#ifndef INCLUDED_SQLWIZARDRY_V1_PREDICATE_HPP
#define INCLUDED_SQLWIZARDRY_V1_PREDICATE_HPP

#include <sqlwizardry/v1/detail/macro.hpp>

namespace sqlwizardry {
template <typename MODEL, typename T>
struct ColumnDefinition;

namespace condition_type {
    struct binary{};
    struct unary{};
}
namespace condition_category {
    struct compound{};
    struct terminal{};
}


SQLWIZARDRY_DEFINE_COMPOUND_BINARY(Or)
SQLWIZARDRY_DEFINE_COMPOUND_BINARY(And)
SQLWIZARDRY_DEFINE_COMPOUND_UNARY(Not)
SQLWIZARDRY_DEFINE_TERMINAL_UNARY(Equals)
SQLWIZARDRY_DEFINE_TERMINAL_UNARY(NotEquals)
SQLWIZARDRY_DEFINE_TERMINAL_UNARY(LessThan)
SQLWIZARDRY_DEFINE_TERMINAL_UNARY(LessThanOrEqual)
SQLWIZARDRY_DEFINE_TERMINAL_UNARY(GreaterThan)
SQLWIZARDRY_DEFINE_TERMINAL_UNARY(GreaterThanOrEqual)
SQLWIZARDRY_DEFINE_TERMINAL_UNARY(In)
SQLWIZARDRY_DEFINE_TERMINAL_UNARY(Like)
SQLWIZARDRY_DEFINE_TERMINAL_UNARY(Between)


template <typename Condition>
struct ColumnPredicate {
    Condition condition;

    explicit constexpr ColumnPredicate(Condition cond) : condition{std::move(cond)} {}

    template <typename ConditionOther>
    [[nodiscard]] constexpr auto operator&(const ColumnPredicate<ConditionOther>& other) {
        return ColumnPredicate<And<Condition, ConditionOther>>{And{condition, other.condition}};
    }
    template <typename ConditionOther>
    [[nodiscard]] constexpr auto operator&&(const ColumnPredicate<ConditionOther>& other) {
        return *this & other;
    }
    template <typename ConditionOther>
    [[nodiscard]] constexpr auto operator|(const ColumnPredicate<ConditionOther>& other) {
        return ColumnPredicate<Or<Condition, ConditionOther>>{Or{condition, other.condition}};
    }
    template <typename ConditionOther>
    [[nodiscard]] constexpr auto operator||(const ColumnPredicate<ConditionOther>& other) {
        return *this | other;
    }

    [[nodiscard]] constexpr auto operator!() {
        return ColumnPredicate<Not<Condition>>{Not{condition}};
    }
};

template <typename Condition>
ColumnPredicate(Condition&&) -> ColumnPredicate<std::decay_t<Condition>>;
}

#endif