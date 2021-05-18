#ifndef INCLUDED_SQLWIZARDRY_V1_DETAIL_TYPE_HPP
#define INCLUDED_SQLWIZARDRY_V1_DETAIL_TYPE_HPP

#include <type_traits>

namespace sqlwizardry
{
struct empty
{
};

struct select_tag
{
};
struct where_tag
{
};
struct order_tag
{
};
struct limit_tag
{
};
struct offset_tag
{
};

template <typename K, typename V>
struct KVType
{
  using key_type = K;
  using value_type = V;
  V value;
  constexpr KVType(V val) : value{std::move(val)} {}
};

template <typename K, typename V>
constexpr auto kv(V&& value)
{
  return KVType<K, std::decay_t<V>>{std::forward<V>(value)};
}

template <typename... V>
struct type_of;

template <typename TAG>
struct type_of<TAG>
{
  using type = KVType<TAG, empty>;
};
template <typename TAG, typename K, typename V>
struct type_of<TAG, KVType<K, V>>
{
  using type = std::decay_t<std::conditional_t<std::is_same_v<TAG, K>, KVType<K, V>, KVType<K, empty>>>;
};
template <typename TAG, typename K, typename V, typename... REST>
struct type_of<TAG, KVType<K, V>, REST...>
{
  using type = std::decay_t<std::conditional_t<std::is_same_v<TAG, K>, KVType<K, V>, typename type_of<TAG, REST...>::type>>;
};

template <typename TAG, typename... T>
using type_of_t = typename type_of<TAG, T...>::type;

template <typename TAG>
constexpr auto value_of()
{
  return empty{};
}

template <typename TAG, template <typename, typename> class KVTypeT, typename K, typename V, typename... REST>
constexpr auto value_of(KVTypeT<K, V> kv, REST&&... rest)
{
  if constexpr (std::is_same_v<TAG, K>)
  {
    return kv.value;
  }
  else
  {
    return value_of<TAG>(std::forward<REST>(rest)...);
  }
}
}  // namespace sqlwizardry

#endif