
#ifndef INCLUDED_SQLWIZARDRY_V1_SERIALISER_HPP
#define INCLUDED_SQLWIZARDRY_V1_SERIALISER_HPP

#include <type_traits>
#include <ostream>
#include <iomanip>

#include <sqlwizardry/v1/detail/traits.hpp>
#include <sqlwizardry/v1/detail/macro.hpp>
#include <sqlwizardry/v1/predicate.hpp>

namespace sqlwizardry {
    SQLWIZARDRY_DECLARE_BEGIN_ELEMENT_SERIALISER(select_element)
    ss << element.table_name << "." << element.name;
    SQLWIZARDRY_DECLARE_END_ELEMENT_SERIALISER

    SQLWIZARDRY_DECLARE_BEGIN_SERIALISER(select)
    ss << "SELECT ";
    std::apply([&ss](const auto& ...args) {
            int column = 0;
            int dummy[] = {
                ((select_element{args}(ss), column++ ? ss : ss << ", "),0)...
            };
        },
        query.get_select()
    );
    SQLWIZARDRY_DECLARE_END_SERIALISER



    SQLWIZARDRY_DECLARE_BEGIN_ELEMENT_SERIALISER(from_element)
    ss << element.table_name;
    SQLWIZARDRY_DECLARE_END_ELEMENT_SERIALISER

    SQLWIZARDRY_DECLARE_BEGIN_SERIALISER(from)
    ss << " FROM ";
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
        ss << " (";
        where_element{element.condition_a}(ss);
        ss << " OR ";
        where_element{element.condition_b}(ss);
        ss << ")";
    SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER

    SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(where_element, 
    (typename T1, typename T2), 
    (And<T1, T2>), 
    (And<T1, T2>, std::void_t<>))
        ss << " (";
        where_element<std::decay_t<T1>>{element.condition_a}(ss);
        ss << " AND ";
        where_element<std::decay_t<T2>>{element.condition_b}(ss);
        ss << ")";
    SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER

    SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(where_element, 
    (typename T), 
    (Not<T>), 
    (Not<T>, std::void_t<>))
        ss << " NOT (";
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
    using where_t = typename type_of_t<where_tag, ELEMENTS...>::value_type;
    if constexpr(!is_empty_v<where_t>) {
        ss << " WHERE ";
        where_element<std::decay_t<where_t>>{query.get_where()}(ss);
    }
    SQLWIZARDRY_DECLARE_END_SERIALISER

    SQLWIZARDRY_DECLARE_TYPE_SPECIALISED_ELEMENT_SERIALISER(order_element, (typename))

    SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(order_element, 
    (typename T1), 
    (column::Ascending<T1>), 
    (column::Ascending<T1>))
        ss << element.column.table_name << "." << element.column.name << " ASC";
    SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER

    SQLWIZARDRY_BEGIN_TYPE_SPECIALISATION_ELEMENT_SERIALISER(order_element, 
    (typename T1), 
    (column::Descending<T1>), 
    (column::Descending<T1>))
        ss << element.column.table_name << "." << element.column.name << " DESC";
    SQLWIZARDRY_END_TYPE_SPECIALISATION_ELEMENT_SERIALISER

    SQLWIZARDRY_DECLARE_BEGIN_SERIALISER(order)
    using order_t = typename type_of_t<order_tag, ELEMENTS...>::value_type;
    if constexpr(!is_empty_v<order_t>) {
        ss << " ORDER BY ";
        std::apply([&ss](const auto& ...order) {
            int column = 0;
            int dummy[] = {
                ((order_element<std::decay_t<decltype(order)>>{order}(ss), !column++ ? ss : ss << ", "),0)...
            };
        },query.get_order());
    }
    SQLWIZARDRY_DECLARE_END_SERIALISER

    SQLWIZARDRY_DECLARE_BEGIN_SERIALISER(limit)
    using limit_t = typename type_of_t<limit_tag, ELEMENTS...>::value_type;
    if constexpr(!is_empty_v<limit_t>) {
        ss << " LIMIT " << query.get_limit();
    }
    SQLWIZARDRY_DECLARE_END_SERIALISER

    SQLWIZARDRY_DECLARE_BEGIN_SERIALISER(offset)
    using offset_t = typename type_of_t<offset_tag, ELEMENTS...>::value_type;
    if constexpr(!is_empty_v<offset_t>) {
        ss << " OFFSET " << query.get_offset();
    }
    SQLWIZARDRY_DECLARE_END_SERIALISER

template <typename MODEL, typename DB, typename ...ELEMENTS>
struct query_serialiser {
    const Query<MODEL, DB, ELEMENTS...>& query;
    query_serialiser(const Query<MODEL, DB, ELEMENTS...>& q) : query{q} {}

    auto operator()(std::ostream& ss) {
        select{query}(ss);
        from{query}(ss);
        where{query}(ss);
        order{query}(ss);
        limit{query}(ss);
        offset{query}(ss);
    }

};
}

#endif