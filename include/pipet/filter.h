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

#include "helpers/reflect.h"
#include "helpers/typelist.h"

namespace pipet {
// Filter types
struct filter_gen;      // data generator
struct filter_proc;     // data processor
struct filter_rev_proc; // reversible data processor

namespace detail {
static constexpr auto has_reverse = helpers::is_valid(
    [](auto x, auto &&arg) -> decltype((void)value_t(x).reverse(value_t(arg))) {
    });

template <typename F> struct filter_traits_impl {
  // return type
  using ret_type = decltype(helpers::function_ret(&F::process));

  // args type intro
  using args_types = decltype(helpers::function_args(&F::process));
  static constexpr auto args_size =
      helpers::size_v<args_types>; // intermediate step due to visual compiler
                                   // failure
  static_assert(args_size <= 1, "[-][pipet] only one-to-one filter handled");

  using arg_type = typename helpers::robust_front<args_types>::type;

  // filter type
  static constexpr auto is_reversible =
      has_reverse(helpers::type<F>, helpers::type<ret_type>);
};

template <typename F,
          bool G = std::is_same_v<helpers::nonsuch,
                                  typename filter_traits_impl<F>::arg_type>>
struct filter_type_selector;

template <typename F> struct filter_type_selector<F, true> {
  using type = filter_gen;
};

template <typename F> struct filter_type_selector<F, false> {
  using type = std::conditional_t<filter_traits_impl<F>::is_reversible,
                                  filter_rev_proc, filter_proc>;
};

template <typename F>
using filter_type_selector_t = typename filter_type_selector<F>::type;
} // namespace detail

namespace traits {
template <typename F> struct filter_traits {
  using ret_type =
      std::decay_t<typename detail::filter_traits_impl<F>::ret_type>;
  using arg_type =
      std::decay_t<typename detail::filter_traits_impl<F>::arg_type>;
  using filter_type = detail::filter_type_selector_t<F>;
};
} // namespace traits

namespace concept {
  template <typename F, typename R,
            typename T = typename traits::filter_traits<F>::filter_type>
  struct io_checker;

  template <typename F, typename R> struct io_checker<F, R, filter_proc> {
    using r_arg_type = typename traits::filter_traits<R>::arg_type;
    using f_ret_type = typename traits::filter_traits<F>::ret_type;

    static constexpr bool value = std::is_convertible_v<f_ret_type, r_arg_type>;
  };

  template <typename F, typename R>
  struct io_checker<F, R, filter_rev_proc> : io_checker<F, R, filter_proc> {
    using r_arg_type = typename traits::filter_traits<R>::arg_type;
    using f_ret_type = typename traits::filter_traits<F>::ret_type;

    static constexpr bool value =
        std::is_convertible_v<r_arg_type, f_ret_type> &&
        io_checker<F, R, filter_proc>::value;
  };

  template <typename F, typename R> constexpr bool io_compatible() {
    return io_checker<F, R>::value;
  }
} // namespace concept

} // namespace pipet