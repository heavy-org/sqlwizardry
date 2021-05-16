#ifndef INCLUDED_SQLWIZARDRY_V1_MACRO_HPP
#define INCLUDED_SQLWIZARDRY_V1_MACRO_HPP

#include <sqlwizardry/v1/detail/type.hpp>
#include <sqlwizardry/v1/column.hpp>

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
    auto& get() \
    { \
        return self.SQLWIZARDRY_COLUMN_NAME(x); \
    }\
    const auto & get() const \
    { \
        return self.SQLWIZARDRY_COLUMN_NAME(x); \
    }\
    static constexpr const auto & get_definition() \
    { \
        return _class_id::SQLWIZARDRY_COLUMN_NAME(x); \
    }\
    const char * name() const \
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
#define SQLWIZARDRY_DECLARE_STATIC_COLUMN_REFLECT(r, _class_id, i, x) \
template<> \
struct _class_id::column_data<i> \
{ \
    static constexpr const auto & get() \
    { \
        return _class_id::SQLWIZARDRY_COLUMN_NAME(x); \
    }\
    static constexpr const char * name() \
    {\
        return BOOST_PP_STRINGIZE(SQLWIZARDRY_COLUMN_NAME(x)); \
    } \
};
#define SQLWIZARDRY_DECLARE_STATIC_COLUMNS_REFLECTION_DATA(_class_id, ...) \
BOOST_PP_SEQ_FOR_EACH_I(SQLWIZARDRY_DECLARE_STATIC_COLUMN_REFLECT, _class_id, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
#define SQLWIZARDRY_TABLE(_class_id, _table_name, ...) \
struct _class_id : sqlwizardry::model<_class_id> { \
    constexpr static std::string_view table_name = _table_name; \
    static constexpr std::size_t number_of_columns = BOOST_PP_VARIADIC_SIZE(__VA_ARGS__); \
    friend struct sqlwizardry::column_reflector<_class_id>; \
    template<std::size_t N> \
    struct column_data {}; \
    SQLWIZARDRY_DECLARE_STATIC_COLUMNS(_class_id,__VA_ARGS__) \
    struct __instance{\
        using column_definition = _class_id; \
        static constexpr std::size_t number_of_fields = BOOST_PP_VARIADIC_SIZE(__VA_ARGS__); \
        friend struct sqlwizardry::field_reflector; \
        template<std::size_t N, class Self> \
        struct field_data {}; \
        SQLWIZARDRY_DECLARE_INSTANCE_COLUMNS(_class_id,__VA_ARGS__) \
    }; \
}; \
SQLWIZARDRY_DECLARE_STATIC_COLUMNS_REFLECTION_DATA(_class_id, __VA_ARGS__)

#endif