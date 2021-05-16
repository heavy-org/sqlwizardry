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
    
}

template <typename... COLUMNS>
constexpr auto get_ordering_tuple(COLUMNS... cols) {
    return std::make_tuple(detail::get_ordering(std::move(cols))...);
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
            auto fields = get_columns<MODEL>();
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

template <typename MODEL, typename DB, typename SELECT, typename WHERE, typename ORDER_BY, typename LIMIT>
struct query_serialiser;

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
}

#endif