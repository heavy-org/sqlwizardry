#ifndef INCLUDED_SQLWIZARDRY_V1_QUERY_HPP
#define INCLUDED_SQLWIZARDRY_V1_QUERY_HPP

#include <tuple>
#include <utility>
#include <sstream>
#include <type_traits>

#include <sqlwizardry/v1/column.hpp>
#include <sqlwizardry/v1/detail/type.hpp>
#include <sqlwizardry/v1/detail/reflection.hpp>

namespace sqlwizardry {

template <typename MODEL, typename DATABASE, typename... ELEMENTS>
struct AllQuery; 

template <typename MODEL, typename DATABASE, typename... ELEMENTS>
class Query;

namespace detail {
    template <typename COLUMNS>
    constexpr auto get_ordering(COLUMNS column) {
        if constexpr(is_ordering_v<std::decay_t<COLUMNS>>) {
            return column;
        }
        else {
            return column::Ascending{std::move(column)};
        }
    }

    template <typename... COLUMNS>
    constexpr auto get_ordering_tuple(COLUMNS... cols) {
        return std::make_tuple(get_ordering(std::move(cols))...);
    }
    

    template <typename MODEL, typename DB, typename... T>
    constexpr auto make_query(DB& db, T&& ...ELEMENTS) {
        return Query<MODEL, DB, std::decay_t<T>...>{db, std::forward<T>(ELEMENTS)...};
    }
}

template <typename... COLUMNS>
constexpr auto get_ordering_tuple(COLUMNS... cols) {
    return std::make_tuple(detail::get_ordering(std::move(cols))...);
}


template <typename MODEL, typename DATABASE, typename... ELEMENTS>
struct AllQuery; 

template <typename MODEL, typename DATABASE, typename... ELEMENTS>
class Query {
    protected:
    DATABASE& db;
    type_of_t<select_tag,ELEMENTS...> _select;
    type_of_t<where_tag,ELEMENTS...> _where;
    type_of_t<order_tag,ELEMENTS...> _order_by;
    type_of_t<limit_tag,ELEMENTS...> _limit;
    type_of_t<offset_tag,ELEMENTS...> _offset;

    public:

    constexpr Query(DATABASE& db, ELEMENTS... e) 
        : db{db}, 
        _select{std::move(value_of<select_tag>(e...))}, 
        _where{std::move(value_of<where_tag>(e...))}, 
        _order_by{std::move(value_of<order_tag>(e...))}, 
        _limit{std::move(value_of<limit_tag>(e...))}, 
        _offset{std::move(value_of<offset_tag>(e...))} {

    }

    template <typename ...COLUMNS>
    [[nodiscard]] constexpr auto select(COLUMNS&& ...cols) {
        static_assert(is_empty_v<decltype(_select.value)>, "Selection already made in this query,");
        if constexpr(sizeof...(cols) == 0) {
            auto fields = get_columns<MODEL>();
            return detail::make_query<MODEL>(db, kv<select_tag>(std::move(fields)), std::move(_where), std::move(_order_by), std::move(_limit),std::move(_offset));
        } else {
            return detail::make_query<MODEL>(db, kv<select_tag>(std::make_tuple(cols...)), std::move(_where), std::move(_order_by), std::move(_limit),std::move(_offset));
        }
    }

    template <typename PREDICATE>
    [[nodiscard]] constexpr auto where(PREDICATE&& pred) {
        if constexpr(is_empty_v<decltype(_where.value)>) {
            return detail::make_query<MODEL>(db, std::move(_select), kv<where_tag>(std::forward<PREDICATE>(pred)), std::move(_order_by), std::move(_limit),std::move(_offset));
        } else {
            return detail::make_query<MODEL>(db, std::move(_select), kv<where_tag>(_where.value && std::forward<PREDICATE>(pred)), std::move(_order_by), std::move(_limit),std::move(_offset));
        }
    }

    template <typename ...ORDERING>
    [[nodiscard]] constexpr auto order_by(ORDERING&& ...order){
        static_assert(is_empty_v<decltype(_order_by.value)>, "This query already has an ordering specified");
        auto ordering = get_ordering_tuple(std::forward<ORDERING>(order)...);
        return detail::make_query<MODEL>(db, std::move(_select), std::move(_where), kv<order_tag>(std::move(ordering)), std::move(_limit),std::move(_offset));
    }

    [[nodiscard]] constexpr auto limit(std::size_t l){
        return detail::make_query<MODEL>(db, std::move(_select), std::move(_where), std::move(_order_by), kv<limit_tag>(l),std::move(_offset));
    }

    template <std::size_t L>
    [[nodiscard]] constexpr auto limit(){
        static_assert(L > 0, "Limit must be greater than 0");
        return detail::make_query<MODEL>(db, std::move(_select), std::move(_where), std::move(_order_by), kv<limit_tag>(L),std::move(_offset));
    }

    [[nodiscard]] constexpr auto offset(std::size_t l){
        return detail::make_query<MODEL>(db, std::move(_select), std::move(_where), std::move(_order_by), std::move(_limit), kv<offset_tag>(l));
    }

    template <std::size_t L>
    [[nodiscard]] constexpr auto offset(){
        return detail::make_query<MODEL>(db, std::move(_select), std::move(_where), std::move(_order_by), std::move(_limit),kv<offset_tag>(L));
    }

    [[nodiscard]] constexpr const auto& get_select() const {
        return _select.value;
    }

    [[nodiscard]] constexpr const auto& get_where() const {
        return _where.value;
    }

    [[nodiscard]] constexpr const auto& get_order() const {
        return _order_by.value;
    }

    [[nodiscard]] constexpr const auto& get_limit() const {
        return _limit.value;
    }

    [[nodiscard]] constexpr const auto& get_offset() const {
        return _offset.value;
    }

    [[nodiscard]] constexpr auto& get_db() {
        return db;
    }

    [[nodiscard]] constexpr auto all() {
        static_assert(!is_empty_v<decltype(_select.value)>, "This query needs at leas one selection. Did you forget to call .select()?");
        return AllQuery<MODEL, DATABASE, ELEMENTS...>{*this};
    }
};




template <typename CRTP>
struct model {
    template <typename DB>
    [[nodiscard]] static constexpr auto query(DB& database ) {
        return Query<CRTP,DB>{database};
    }    
};

template <typename MODEL, typename DB, typename... ELEMENTS>
struct query_serialiser;

template <typename MODEL, typename DATABASE, typename... ELEMENTS>
class AllQuery : private Query<MODEL, DATABASE, ELEMENTS...>
{
    using QUERY_T = Query<MODEL, DATABASE, ELEMENTS...>;
    public:

    constexpr AllQuery(Query<MODEL, DATABASE, ELEMENTS...> q) : QUERY_T{std::move(q)} {}

    auto begin() {
        std::stringstream ss;
        query_serialiser{*this}(ss);
        auto& db = QUERY_T::get_db();
        db.run(ss.str());
    }
    
    auto end() {

    }
};
}

#endif