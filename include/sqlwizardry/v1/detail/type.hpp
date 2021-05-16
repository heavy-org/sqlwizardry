#ifndef INCLUDED_SQLWIZARDRY_V1_DETAIL_TYPE_HPP
#define INCLUDED_SQLWIZARDRY_V1_DETAIL_TYPE_HPP

namespace sqlwizardry {
struct empty{};

struct select_tag {};
struct where_tag {};
struct order_tag {};
struct limit_tag {};
    template <typename TYPE, typename... ARGS>
    struct Column : ARGS... {
        using type = std::decay_t<TYPE>;
    };
}

#endif