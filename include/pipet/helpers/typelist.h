// Copyright 2018 Ken Avolic <kenavolic@none.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "utils.h"

#include <type_traits>

namespace pipet::helpers {
// generic typelist features

template <typename List> struct is_empty;

template <template <typename...> typename List, typename... Items>
struct is_empty<List<Items...>> : std::false_type {};

template <template <typename...> typename List>
struct is_empty<List<>> : std::true_type {};

template <typename List> using is_empty_t = typename is_empty<List>::type;

template <typename List> constexpr bool is_empty_v = is_empty_t<List>::value;

template <typename List> struct size;

template <template <typename...> typename List, typename... Items>
struct size<List<Items...>> {
  static constexpr std::size_t value = sizeof...(Items);
};

template <template <typename...> typename List> struct size<List<>> {
  static constexpr std::size_t value = 0;
};

template <typename List> constexpr auto size_v = size<List>::value;

template <typename List> struct front;

template <template <typename...> typename List, typename Head, typename... Tail>
struct front<List<Head, Tail...>> {
  using type = Head;
};

template <typename List> using front_t = typename front<List>::type;

template <typename List> struct robust_front { using type = front_t<List>; };

template <template <typename...> typename List> struct robust_front<List<>> {
  using type = nonsuch;
};

template <typename List>
using robust_front_t = typename robust_front<List>::type;

template <typename List> struct pop_front;

template <template <typename...> typename List, typename Head, typename... Tail>
struct pop_front<List<Head, Tail...>> {
  using type = List<Tail...>;
};

template <template <typename...> typename List> struct pop_front<List<>> {
  using type = List<>;
};

template <typename List> using pop_front_t = typename pop_front<List>::type;

template <typename Item, typename List> struct push_front;

template <typename Item, template <typename...> typename List,
          typename... Items>
struct push_front<Item, List<Items...>> {
  using type = List<Item, Items...>;
};

template <typename Item, typename List>
using push_front_t = typename push_front<Item, List>::type;

template <typename List> struct pop_back;

template <template <typename...> typename List, typename Head, typename... Tail>
struct pop_back<List<Head, Tail...>> {
  using type = push_front_t<Head, typename pop_back<List<Tail...>>::type>;
};

template <template <typename...> typename List, typename Head>
struct pop_back<List<Head>> {
  using type = List<>;
};

template <typename List> using pop_back_t = typename pop_back<List>::type;

namespace detail {
template <typename Item, typename List, bool = is_empty_v<List>>
struct push_back_impl;

template <typename Item, typename List>
struct push_back_impl<Item, List, false> {
  using type =
      push_front_t<front_t<List>,
                   typename push_back_impl<Item, pop_front_t<List>>::type>;
};

template <typename Item, typename List>
struct push_back_impl<Item, List, true> {
  using type = push_front_t<Item, List>;
};
} // namespace detail

template <typename Item, typename List>
struct push_back : detail::push_back_impl<Item, List> {};

template <typename Item, typename List>
using push_back_t = typename push_back<Item, List>::type;

template <typename Item, typename List> struct has {
  static constexpr bool value =
      !is_empty_v<List> && (std::is_same_v<Item, robust_front_t<List>> ||
                            has<Item, pop_front_t<List>>::value);
};

template <typename Item, typename List>
constexpr bool has_v = has<Item, List>::value;

template <typename List> struct remove_dup;

template <template <typename...> typename List, typename Head, typename... Tail>
struct remove_dup<List<Head, Tail...>> {
  using type = std::conditional_t<
      has_v<Head, List<Tail...>>, typename remove_dup<List<Tail...>>::type,
      push_front_t<Head, typename remove_dup<List<Tail...>>::type>>;
};

template <template <typename...> typename List, typename Head>
struct remove_dup<List<Head>> {
  using type = List<Head>;
};

template <typename List> using remove_dup_t = typename remove_dup<List>::type;

template <typename List1, typename List2, bool = is_empty_v<List2>>
struct concat;

template <typename List1, typename List2> struct concat<List1, List2, false> {
  using type = push_front_t<front_t<List2>,
                            typename concat<List1, pop_front_t<List2>>::type>;
};

template <typename List1, typename List2> struct concat<List1, List2, true> {
  using type = List1;
};

template <typename List1, typename List2>
using concat_t = typename concat<List1, List2>::type;

template <typename List1, typename List2>
using merge_t = remove_dup_t<concat_t<List1, List2>>;

template <typename... Lists> struct concat_all;

template <typename L1, typename L2, typename... Ls>
struct concat_all<L1, L2, Ls...> {
  using type = concat_t<L1, typename concat_all<L2, Ls...>::type>;
};

template <typename L> struct concat_all<L> { using type = L; };

template <typename... Lists>
using concat_all_t = typename concat_all<Lists...>::type;

template <typename... Lists>
using merge_all_t = remove_dup_t<concat_all_t<Lists...>>;

// rebind
template <template <typename...> typename TList, typename SList> struct rebind;

template <template <typename...> typename TList,
          template <typename...> typename SList, typename... Args>
struct rebind<TList, SList<Args...>> {
  using type = TList<Args...>;
};

template <template <typename...> typename TList, typename SList>
using rebind_t = typename rebind<TList, SList>::type;

// basic typelist
template <typename... Ts> struct typelist {};
} // namespace pipet::helpers