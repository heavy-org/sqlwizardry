#include <functional>
#include <string>
#include <optional>
#include <unordered_map>
#include <iostream>
#include <string>
#include <vector>
#include <string_view>
#include <memory>
#include <variant>
#include <sstream>
#include <tuple>
#include <iomanip>

#include <experimental/array>

#include <boost/preprocessor.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/bind/bind.hpp>


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

template <typename T>
using is_string = detail::is_string<std::decay_t<T>>;

namespace column{
    struct PrimaryKey {};
    struct Required {};
    struct ForiegnKey {};
    struct Unique {};
}

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

template <typename TYPE, typename... ARGS>
struct Column : ARGS... {
    using type = std::decay_t<TYPE>;
};

template <typename T>
struct is_column : std::false_type {};

template <typename... REST>
struct is_column<Column<REST...>> : std::true_type {};

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

template <typename T>
struct is_ordering : std::false_type {};
template <typename ...Columns>
struct is_ordering<Ascending<Columns...>> : std::true_type {};
template <typename ...Columns>
struct is_ordering<Descending<Columns...>> : std::true_type {};
template <typename T>
constexpr auto is_ordering_v = is_ordering<T>::value;
template <typename T>
struct is_ascending : std::false_type {};
template <typename ...Columns>
struct is_ascending<Ascending<Columns...>> : std::true_type {};
template <typename T>
constexpr auto is_ascending_v = is_ascending<T>::value;
template <typename T>
struct is_descending : std::false_type {};
template <typename ...Columns>
struct is_descending<Descending<Columns...>> : std::true_type {};
template <typename T>
constexpr auto is_descending_v = is_descending<T>::value;

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

#define SQLWIZARDRY_DEFINE_COMPOUND_BINARY(X)\
template <typename A, typename B>\
struct X {\
    using type = condition_type::binary;\
    using category = condition_category::compound;\
    A condition_a;\
    B condition_b;\
    constexpr X(A a, B b) : condition_a{std::move(a)}, condition_b{std::move(b)} {} \
};
#define SQLWIZARDRY_DEFINE_COMPOUND_UNARY(X)\
template <typename T>\
struct X {\
    using type = condition_type::unary;\
    using category = condition_category::compound;\
    T condition;\
    explicit constexpr X(T cond) : condition{std::move(cond)} {}\
};
#define SQLWIZARDRY_DEFINE_TERMINAL_UNARY(X) \
template <typename MODEL, typename T, typename T1>\
struct X {\
    using type = condition_type::unary;\
    using category = condition_category::terminal;\
    const ColumnDefinition<MODEL, T>& column;\
    T1 value;\
    constexpr X(const ColumnDefinition<MODEL, T>& col, T1 val) : column{std::move(col)}, value{std::move(val)} {}\
};

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

    explicit constexpr ColumnPredicate(const Condition& cond) : condition{cond} {}

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


#define SQLWIZARDRY_DEFINE_COMPOUND_TERMINAL_OPERATOR_MAPPING(X, Operator) \
[[nodiscard]] constexpr auto operator Operator(const type& criteria) const { \
    return ColumnPredicate{X{*this, criteria}};\
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
#define EAT(...)
#define SQLWIZARDRY_ESC(...) __VA_ARGS__, 
#define SQLWIZARDRY_DEFER(x) x BOOST_PP_EMPTY()
#define SQLWIZARDRY_COLUMN_TYPE4(x) \
BOOST_PP_TUPLE_REM()x
#define SQLWIZARDRY_COLUMN_TYPE3(x) \
SQLWIZARDRY_COLUMN_TYPE4(BOOST_PP_TUPLE_POP_BACK(x))
#define SQLWIZARDRY_COLUMN_TYPE2(x) \
SQLWIZARDRY_COLUMN_TYPE3(BOOST_PP_VARIADIC_TO_TUPLE(x))
#define SQLWIZARDRY_COLUMN_TYPE(x) \
SQLWIZARDRY_COLUMN_TYPE2(SQLWIZARDRY_ESC x)
#define SQLWIZARDRY_COLUMN_NAME(x) \
BOOST_PP_TUPLE_EAT()x
#define SQLWIZARDRY_DECLARE_INSTANCE_COLUMN(r, _class_id, i, x) \
typename SQLWIZARDRY_COLUMN_TYPE(x)::type SQLWIZARDRY_COLUMN_NAME(x);\
template<class Self> \
struct field_data<i, Self> \
{ \
    Self & self; \
    field_data(Self & self) : self(self) {} \
    \
    const auto& get_member() \
    { \
        return self.SQLWIZARDRY_COLUMN_NAME(x); \
    }\
    const auto & get_member() const \
    { \
        return self.SQLWIZARDRY_COLUMN_NAME(x); \
    }\
    static constexpr const auto & get() \
    { \
        return _class_id::SQLWIZARDRY_COLUMN_NAME(x); \
    }\
    const char * member_name() const \
    {\
        return BOOST_PP_STRINGIZE(SQLWIZARDRY_COLUMN_NAME(x)); \
    } \
    static constexpr const char * name() \
    {\
        return BOOST_PP_STRINGIZE(SQLWIZARDRY_COLUMN_NAME(x)); \
    } \
}; 
#define SQLWIZARDRY_DECLARE_INSTANCE_COLUMNS(_class_id, ...) \
BOOST_PP_SEQ_FOR_EACH_I(SQLWIZARDRY_DECLARE_INSTANCE_COLUMN, _class_id, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
#define SQLWIZARDRY_DECLARE_STATIC_COLUMN(r, _class_id, i, x) \
constexpr static ColumnDefinition<_class_id,SQLWIZARDRY_COLUMN_TYPE(x)> SQLWIZARDRY_COLUMN_NAME(x){BOOST_PP_STRINGIZE(SQLWIZARDRY_COLUMN_NAME(x))};
#define SQLWIZARDRY_DECLARE_STATIC_COLUMNS(_class_id, ...) \
BOOST_PP_SEQ_FOR_EACH_I(SQLWIZARDRY_DECLARE_STATIC_COLUMN, _class_id, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
#define SQLWIZARDRY_TABLE(_class_id, _table_name, ...) \
struct _class_id : sqlwizardry::model<_class_id> { \
    constexpr static std::string_view table_name = _table_name; \
    static constexpr std::size_t number_of_fields = BOOST_PP_VARIADIC_SIZE(__VA_ARGS__); \
    SQLWIZARDRY_DECLARE_STATIC_COLUMNS(_class_id,__VA_ARGS__) \
    struct __instance{\
        using column_definition = _class_id; \
        friend struct sqlwizardry::reflector; \
        template<std::size_t N, class Self> \
        struct field_data {}; \
        SQLWIZARDRY_DECLARE_INSTANCE_COLUMNS(_class_id,__VA_ARGS__) \
    }; \
};
class reflector
{
    private:
        
    public:
    //Get field_data at index N
    template<int N, class T>
    [[nodiscard]] constexpr static typename T::template field_data<N, T> get_field_data(T& x)
    {
        return typename T::template field_data<N, T>(x);
    }

    // Get the number of fields
    template<class T>
    struct fields
    {
        static constexpr int n = T::fields_n;
    };
};

struct field_visitor
{
    template<class C, class Visitor, class I>
    void operator()(C& c, Visitor v, I)
    {
        v(reflector::get_field_data<I::value>(c));
    }
};



template<class C, class Visitor>
constexpr void visit_each(C & c, Visitor v)
{
    using namespace boost::placeholders;
    using range = boost::mpl::range_c<int,0,reflector::fields<C>::n>;
    boost::mpl::for_each<range>(boost::bind<void>(field_visitor(), boost::ref(c), v, _1));
}

namespace detail {
    template <typename C, typename IntType, IntType... Idxs>
    [[nodiscard]] constexpr auto get_field_names(std::integer_sequence<IntType, Idxs...>)
    {
        return std::make_tuple(C::template field_data<Idxs, C>::name()...);
    }

    template <typename MODEL, typename IntType, IntType... Idxs>
    [[nodiscard]] constexpr auto get_fields(std::integer_sequence<IntType, Idxs...>)
    {
        return std::make_tuple(MODEL::template field_data<Idxs, MODEL>::get()...);
    }
}

template<class C>
[[nodiscard]] constexpr auto get_fields()
{
    return detail::get_fields<C>(std::make_index_sequence<reflector::fields<C>::n>());
}

template<typename T>
[[nodiscard]] constexpr auto get_field_names(T&&)
{
    static_assert(sizeof(T) != sizeof(T), "Can only be uses on a tuple of columns");
}

template<template <typename, typename...> class... COLUMNS_T, typename MODEL, typename... REST>
[[nodiscard]] constexpr auto get_field_names(std::tuple<COLUMNS_T<MODEL, REST>...> cols)
{
    return detail::get_field_names(std::make_index_sequence<reflector::fields<MODEL>::n>());
}

struct empty{};

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

namespace detail {
    template <typename ...T, typename IntType, IntType... Idxs>
    constexpr auto concat_tuple(const std::tuple<T...>& tup, std::string_view delim, std::integer_sequence<IntType, Idxs...>) {
        return ( ((Idxs == 0 ? "" : delim.data()) + std::get<Idxs>(tup)) + ... );
    }
}

template <typename ...T>
constexpr auto concat_tuple(const std::tuple<T...>& tup, std::string_view delim = " ") {
    return detail::concat_tuple(tup, delim, std::make_index_sequence<sizeof...(T)>());
}

template <typename COLUMNS>
constexpr auto get_ordering(COLUMNS column) {
    if constexpr(is_ordering_v<std::decay_t<COLUMNS>>) {
        return column;
    }
    else {
        return Ascending{std::move(column)};
    }
}

template <typename... COLUMNS>
constexpr auto get_ordering_tuple(COLUMNS... cols) {
    return std::make_tuple(get_ordering(std::move(cols))...);
}

template <typename MODEL, typename DATABASE, typename SELECT = empty, typename WHERE = empty, typename ORDER_BY = empty, typename LIMIT = empty>
struct AllQuery; 



template <typename MODEL, typename DATABASE, typename SELECT = empty, typename WHERE = empty, typename ORDER_BY = empty, typename LIMIT = empty>
class Query {
    protected:
    DATABASE& db;
    SELECT _select;
    WHERE _where;
    ORDER_BY _order_by;
    LIMIT _limit;

    public:
    constexpr explicit Query(DATABASE& dbIn) : db{dbIn} {}

    constexpr Query(DATABASE& db, SELECT s, WHERE w, ORDER_BY o, LIMIT l) : db{db}, _select{std::move(s)}, _where{std::move(w)}, _order_by{std::move(o)}, _limit{std::move(l)} {

    }

    template <typename ...COLUMNS>
    [[nodiscard]] constexpr auto select(COLUMNS&& ...cols) {
        static_assert(is_empty_v<SELECT>, "Selection already made in this query,");
        if constexpr(sizeof...(cols) == 0) {
            auto fields = get_fields<MODEL>();
            return Query<MODEL, DATABASE, decltype(fields), WHERE,ORDER_BY, LIMIT>{db, std::move(fields), std::move(_where), std::move(_order_by), std::move(_limit)};
        } else {
            return Query<MODEL, DATABASE, std::tuple<COLUMNS...>, WHERE, ORDER_BY, LIMIT>{db, std::forward_as_tuple(cols...), std::move(_where), std::move(_order_by), std::move(_limit)};
        }
    }

    template <typename PREDICATE>
    [[nodiscard]] constexpr auto where(PREDICATE&& pred) {
        if constexpr(is_empty_v<WHERE>) {
            return Query<MODEL, DATABASE, SELECT,std::decay_t<PREDICATE>, ORDER_BY, LIMIT>{db, std::move(_select), std::forward<PREDICATE>(pred), std::move(_order_by), std::move(_limit)};
        } else {
            using PRED_TYPE = decltype(_where && pred);
            return Query<MODEL, DATABASE, SELECT,PRED_TYPE, ORDER_BY, LIMIT>{db, std::move(_select), _where && std::forward<PREDICATE>(pred), std::move(_order_by), std::move(_limit)};
        }
    }

    template <typename ...ORDERING>
    [[nodiscard]] constexpr auto order_by(ORDERING&& ...order){
        static_assert(is_empty_v<ORDER_BY>, "This query already has an ordering specified");
        auto ordering = get_ordering_tuple(std::forward<ORDERING>(order)...);
        return Query<MODEL, DATABASE, SELECT, WHERE, std::decay_t<decltype(ordering)>, LIMIT>{db, std::move(_select), std::move(_where), std::move(ordering), std::move(_limit)};
    }

    [[nodiscard]] constexpr const auto& get_select() const {
        return _select;
    }

    [[nodiscard]] constexpr const auto& get_where() const {
        return _where;
    }

    [[nodiscard]] constexpr const auto& get_order() const {
        return _order_by;
    }

    [[nodiscard]] constexpr const auto& get_limit() const {
        return _limit;
    }

    [[nodiscard]] constexpr auto& get_db() {
        return db;
    }

    [[nodiscard]] constexpr auto all() {
        static_assert(!is_empty_v<SELECT>, "This query needs at leas one selection. Did you forget to call .select()?");
        return AllQuery<MODEL, DATABASE, SELECT, WHERE, ORDER_BY, LIMIT>{*this};
    }
};




template <typename CRTP>
struct model {
    template <typename DB>
    [[nodiscard]] static constexpr auto query(DB& database ) {
        return Query<CRTP,DB>{database};
    }    
};

#define SQLWIZARDRY_DECLARE_BEGIN_ELEMENT_SERIALISER(name)\
template <typename T>\
struct name {\
    const T& element;\
    constexpr name(const T& col) : element{col} {}\
    void operator()(std::ostream& ss) {
#define SQLWIZARDRY_DECLARE_END_ELEMENT_SERIALISER\
    }\
};

#define SQLWIZARDRY_DECLARE_TYPE_SPECIALISED_ELEMENT_SERIALISER(name, type_params)\
template <BOOST_PP_TUPLE_REM()type_params>\
struct name {};

#define SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(name, type_params, element_type, specialisation)\
template <BOOST_PP_TUPLE_REM()type_params>\
struct name<BOOST_PP_TUPLE_REM()specialisation> {\
    const BOOST_PP_TUPLE_REM()element_type& element;\
    constexpr name(const BOOST_PP_TUPLE_REM()element_type& col) : element{col} {}\
    void operator()(std::ostream& ss) {
#define SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER\
    }\
};

#define SQLWIZARDRY_DECLARE_BEGIN_SERIALISER(name)\
struct name {\
    const Query<MODEL, DB, SELECT, WHERE, ORDER_BY, LIMIT>& query;\
    constexpr name(const Query<MODEL, DB, SELECT, WHERE, ORDER_BY, LIMIT>& q) : query{q} {}\
    void operator()(std::ostream& ss) {
#define SQLWIZARDRY_DECLARE_END_SERIALISER\
    }\
};


template <typename MODEL, typename DB, typename SELECT, typename WHERE, typename ORDER_BY, typename LIMIT>
struct query_serialiser {
    const Query<MODEL, DB, SELECT, WHERE, ORDER_BY, LIMIT>& query;
    query_serialiser(const Query<MODEL, DB, SELECT, WHERE, ORDER_BY, LIMIT>& q) : query{q} {}

    SQLWIZARDRY_DECLARE_BEGIN_ELEMENT_SERIALISER(select_element)
    ss << element.table_name << "." << element.name;
    SQLWIZARDRY_DECLARE_END_ELEMENT_SERIALISER

    SQLWIZARDRY_DECLARE_BEGIN_SERIALISER(select)
    ss << "SELECT ";
    std::apply([&ss](const auto& ...args) {
            int column = 0;
            int dummy[] = {
                ((select_element{args}(ss), !column++ ? ss : ss << ", "),0)...
            };
        },
        query.get_select()
    );
    SQLWIZARDRY_DECLARE_END_SERIALISER



    SQLWIZARDRY_DECLARE_BEGIN_ELEMENT_SERIALISER(from_element)
    ss << element.table_name;
    SQLWIZARDRY_DECLARE_END_ELEMENT_SERIALISER

    SQLWIZARDRY_DECLARE_BEGIN_SERIALISER(from)
    ss << "FROM ";
    from_element{std::get<0>(query.get_select())}(ss);
    SQLWIZARDRY_DECLARE_END_SERIALISER

    SQLWIZARDRY_DECLARE_TYPE_SPECIALISED_ELEMENT_SERIALISER(where_element, (typename,typename=void))

    SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(where_element, 
    (typename T), 
    (ColumnPredicate<T>), 
    (ColumnPredicate<T>, std::void_t<>))
        where_element<std::decay_t<T>>{element.condition}(ss);
    SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER

    SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(where_element, 
    (typename T1, typename T2), 
    (Or<T1, T2>), 
    (Or<T1, T2>, std::void_t<>))
        ss << "(";
        where_element{element.condition_a}(ss);
        ss << " OR ";
        where_element{element.condition_b}(ss);
        ss << ")";
    SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER

    SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(where_element, 
    (typename T1, typename T2), 
    (And<T1, T2>), 
    (And<T1, T2>, std::void_t<>))
        ss << "(";
        where_element<std::decay_t<T1>>{element.condition_a}(ss);
        ss << " AND ";
        where_element<std::decay_t<T2>>{element.condition_b}(ss);
        ss << ")";
    SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER

    SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(where_element, 
    (typename T), 
    (Not<T>), 
    (Not<T>, std::void_t<>))
        ss << "NOT (";
        where_element<std::decay_t<T>>{element.condition}(ss);
        ss << ")";
    SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER

    SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(where_element, 
    (typename M, typename T1, typename T2), 
    (Equals<M, T1, T2>), 
    (Equals<M, T1, T2>, std::void_t<>))
        ss << M::table_name << "." << element.column.name << " = ";
        where_element<std::decay_t<T2>>{element.value}(ss);
    SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER

    SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(where_element, 
    (typename M, typename T1, typename T2), 
    (NotEquals<M, T1, T2>), 
    (NotEquals<M, T1, T2>, std::void_t<>))
        ss << M::table_name << "." << element.column.name << " <> ";
        where_element<std::decay_t<T2>>{element.value}(ss);
    SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER

    SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(where_element, 
    (typename M, typename T1, typename T2), 
    (In<M, T1, T2>), 
    (In<M, T1, T2>, std::void_t<>))
        ss << element.column.table_name << "." << element.column.name << " IN (";
        bool first = true;
        for(const auto& val : element.value){
            if(!first) {
                ss << ",";
            }
            where_element<std::decay_t<decltype(val)>>{val}(ss);
            first = false;
        }
        ss << ")";
    SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER

    SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(where_element, 
    (typename M, typename T1, typename T2), 
    (Not<In<M, T1, T2>>), 
    (Not<In<M, T1, T2>>, std::void_t<>))
        ss << element.condition.column.table_name << "." << element.condition.column.name << " NOT IN (";
        bool first = true;
        for(const auto& val : element.condition.value){
            if(!first) {
                ss << ",";
            }
            where_element<std::decay_t<decltype(val)>>{val}(ss);
            first = false;
        }
        ss << ")";
    SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER

    SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(where_element, 
    (typename T1), 
    (T1), 
    (T1, std::enable_if_t<std::is_arithmetic_v<T1>>))
        ss << element;
    SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER

    SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(where_element, 
    (typename T1), 
    (T1), 
    (T1, std::enable_if_t<is_string<T1>::value>))
        ss << std::quoted(element);
    SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER


    SQLWIZARDRY_DECLARE_BEGIN_SERIALISER(where)
    if constexpr(!is_empty_v<WHERE>) {
        ss << "WHERE ";
        where_element<std::decay_t<WHERE>>{query.get_where()}(ss);
    }
    SQLWIZARDRY_DECLARE_END_SERIALISER

    SQLWIZARDRY_DECLARE_TYPE_SPECIALISED_ELEMENT_SERIALISER(order_element, (typename))

    SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(order_element, 
    (typename T1), 
    (Ascending<T1>), 
    (Ascending<T1>))
        ss << element.column.name << " ASC";
    SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER

    SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(order_element, 
    (typename T1), 
    (Descending<T1>), 
    (Descending<T1>))
        ss << element.column.name << " DESC";
    SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER

    SQLWIZARDRY_DECLARE_BEGIN_SERIALISER(order)
    if constexpr(!is_empty_v<WHERE>) {
        ss << "ORDER BY ";
        std::apply([&ss](const auto& ...order) {
            int column = 0;
            int dummy[] = {
                ((order_element<std::decay_t<decltype(order)>>{order}(ss), !column++ ? ss : ss << ", "),0)...
            };
        },query.get_order());
    }
    SQLWIZARDRY_DECLARE_END_SERIALISER

    auto operator()(std::ostream& ss) {
        select{query}(ss);
        ss << " ";
        from{query}(ss);
        ss << " ";
        where{query}(ss);
        ss << " ";
        order{query}(ss);
    }

};



namespace engine{
struct MySQL {
    std::string name;
    
    auto run(const std::string& query) {
        std::cout << query << std::endl;
        return 1;
    }
};
}

template <typename MODEL, typename DATABASE, typename SELECT, typename WHERE, typename ORDER_BY, typename LIMIT>
class AllQuery : private Query<MODEL, DATABASE, SELECT, WHERE, ORDER_BY, LIMIT>
{
    using QUERY_T = Query<MODEL, DATABASE, SELECT, WHERE, ORDER_BY, LIMIT>;
    public:

    constexpr AllQuery(QUERY_T q) : QUERY_T{std::move(q)} {}

    auto begin() {
        std::stringstream ss;
        query_serialiser{*this}(ss);
        auto& db = QUERY_T::get_db();
        db.run(ss.str());
    }
    
    auto end() {

    }
};

template <typename MODEL, typename ...T>
constexpr auto create(T&& ...args) {
    return typename MODEL::__instance{std::forward<T>(args)...};
}
}