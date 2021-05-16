
#ifndef INCLUDED_SQLWIZARDRY_V1_DETAIL_REFLECTION_HPP
#define INCLUDED_SQLWIZARDRY_V1_DETAIL_REFLECTION_HPP

#include <utility>

#include <boost/mpl/for_each.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/bind/bind.hpp>

namespace sqlwizardry {

class field_reflector
{
    private:
        
    public:
    //Get field_data at index N
    template<std::size_t N, class T>
    [[nodiscard]] constexpr static typename T::template field_data<N, T> get_field_data(T& x)
    {
        return typename T::template field_data<N, T>(x);
    }

    // Get the number of fields
    template<class T>
    struct fields
    {
        static constexpr int n = T::number_of_fields;
    };
};

template <typename T>
class column_reflector
{
    private:
        
    public:
    //Get field_data at index N
    template<std::size_t N>
    struct column {
        using data =typename T::template column_data<N>; 
    };
};

struct field_visitor
{
    template<class C, class Visitor, class I>
    void operator()(C& c, Visitor v, I)
    {
        v(field_reflector::get_field_data<I::value>(c));
    }
};


template<class C, class Visitor>
constexpr void visit_each(C & c, Visitor v)
{
    using namespace boost::placeholders;
    using range = boost::mpl::range_c<int,0,C::number_of_fields>;
    boost::mpl::for_each<range>(boost::bind<void>(field_visitor(), boost::ref(c), v, _1));
}

namespace detail {
    template <typename C, typename IntType, IntType... Idxs>
    [[nodiscard]] constexpr auto get_column_names(std::integer_sequence<IntType, Idxs...>)
    {
        return std::make_tuple(column_reflector<C>::template column<Idxs>::data::name()...);
    }

    template <typename MODEL, typename IntType, IntType... Idxs>
    [[nodiscard]] constexpr auto get_columns(std::integer_sequence<IntType, Idxs...>)
    {
        return std::make_tuple(column_reflector<MODEL>::template column<Idxs>::data::get()...);
    }
    
    template <typename MODEL, typename IntType, IntType... Idxs>
    [[nodiscard]] constexpr auto get_fields(MODEL& item, std::integer_sequence<IntType, Idxs...>)
    {
        return std::make_tuple(MODEL::field_data<Idxs>(item).get()...);
    }
}

template<class C>
[[nodiscard]] constexpr auto get_fields(C& instance)
{
    return detail::get_fields(instance, std::make_index_sequence<C::number_of_fields>());
}
template<class C>
[[nodiscard]] constexpr auto get_columns()
{
    return detail::get_columns<C>(std::make_index_sequence<C::number_of_columns>());
}
template<class C>
[[nodiscard]] constexpr auto get_column_names()
{
    return detail::get_column_names<C>(std::make_index_sequence<C::number_of_columns>());
}


template <typename MODEL, typename ...T>
constexpr auto create(T&& ...args) {
    return typename MODEL::__instance{std::forward<T>(args)...};
}
}

#endif