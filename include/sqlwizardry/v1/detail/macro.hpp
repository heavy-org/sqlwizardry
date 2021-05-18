#ifndef INCLUDED_SQLWIZARDRY_V1_DETAIL_MACRO_HPP
#define INCLUDED_SQLWIZARDRY_V1_DETAIL_MACRO_HPP

#include <boost/preprocessor/empty.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple/eat.hpp>
#include <boost/preprocessor/tuple/pop_back.hpp>
#include <boost/preprocessor/tuple/rem.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/variadic/to_tuple.hpp>

// Helpers for generating definitions of named types to describe the AST.
#define SQLWIZARDRY_DEFINE_COMPOUND_BINARY(X)                                       \
  template <typename A, typename B>                                                 \
  struct X                                                                          \
  {                                                                                 \
    using type = sqlwizardry::condition_type::binary;                               \
    using category = sqlwizardry::condition_category::compound;                     \
    A condition_a;                                                                  \
    B condition_b;                                                                  \
    constexpr X(A a, B b) : condition_a{std::move(a)}, condition_b{std::move(b)} {} \
  };
#define SQLWIZARDRY_DEFINE_COMPOUND_UNARY(X)                     \
  template <typename T>                                          \
  struct X                                                       \
  {                                                              \
    using type = sqlwizardry::condition_type::unary;             \
    using category = sqlwizardry::condition_category::compound;  \
    T condition;                                                 \
    explicit constexpr X(T cond) : condition{std::move(cond)} {} \
  };
#define SQLWIZARDRY_DEFINE_TERMINAL_UNARY(X)                                                                                   \
  template <typename MODEL, typename T, typename T1>                                                                           \
  struct X                                                                                                                     \
  {                                                                                                                            \
    using type = sqlwizardry::condition_type::unary;                                                                           \
    using category = sqlwizardry::condition_category::terminal;                                                                \
    const sqlwizardry::ColumnDefinition<MODEL, T>& column;                                                                     \
    T1 value;                                                                                                                  \
    constexpr X(const sqlwizardry::ColumnDefinition<MODEL, T>& col, T1 val) : column{std::move(col)}, value{std::move(val)} {} \
  };

// Helper to map an operator to a AST type.
#define SQLWIZARDRY_DEFINE_COMPOUND_TERMINAL_OPERATOR_MAPPING(X, Operator) \
  [[nodiscard]] constexpr auto operator Operator(const type& criteria) const { return sqlwizardry::ColumnPredicate{X{*this, criteria}}; }

#define SQLWIZARDRY_DECLARE_BEGIN_ELEMENT_SERIALISER(name) \
  template <typename T>                                    \
  struct name                                              \
  {                                                        \
    const T& element;                                      \
    constexpr name(const T& col) : element{col} {}         \
    template <typename STREAM_T>                           \
    void operator()(STREAM_T& ss)                          \
    {
#define SQLWIZARDRY_DECLARE_END_ELEMENT_SERIALISER \
  }                                                \
  }                                                \
  ;

#define SQLWIZARDRY_DECLARE_TYPE_SPECIALISED_ELEMENT_SERIALISER(name, type_params) \
  template <BOOST_PP_TUPLE_REM() type_params>                                      \
  struct name                                                                      \
  {                                                                                \
  };

#define SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(name, type_params, element_type, specialisation) \
  template <BOOST_PP_TUPLE_REM() type_params>                                                                     \
  struct name<BOOST_PP_TUPLE_REM() specialisation>                                                                \
  {                                                                                                               \
    const BOOST_PP_TUPLE_REM() element_type& element;                                                             \
    constexpr name(const BOOST_PP_TUPLE_REM() element_type& col) : element{col} {}                                \
    template <typename STREAM_T>                                                                                  \
    void operator()(STREAM_T& ss)                                                                                 \
    {
#define SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER \
  }                                                            \
  }                                                            \
  ;

#define SQLWIZARDRY_DECLARE_BEGIN_SERIALISER(name)                                    \
  template <typename MODEL, typename DB, typename... ELEMENTS>                        \
  struct name                                                                         \
  {                                                                                   \
    const Query<MODEL, DB, ELEMENTS...>& query;                                       \
    constexpr name(const sqlwizardry::Query<MODEL, DB, ELEMENTS...>& q) : query{q} {} \
    template <typename STREAM_T>                                                      \
    void operator()(STREAM_T& ss)                                                     \
    {
#define SQLWIZARDRY_DECLARE_END_SERIALISER \
  }                                        \
  }                                        \
  ;

#endif
