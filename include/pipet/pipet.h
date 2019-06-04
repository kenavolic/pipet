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

#include "filter.h"
#include "helpers/typelist.h"
#include "helpers/utils.h"

#include <type_traits>

namespace pipet {

namespace detail {
template <typename F, typename R, typename T> struct regular_element_impl;

template <typename F, typename R>
struct regular_element_impl<F, R, filter_gen> {
  static constexpr auto process() { return R::process(F::process()); }
};

template <typename F, typename R, typename Args>
struct regular_element_impl_varargs;

template <typename F, typename R, template <typename...> typename List,
          typename... Args>
struct regular_element_impl_varargs<F, R, List<Args...>>
    : helpers::requires_v<concept ::io_compatible<F, R>()> {
  static constexpr auto process(Args... args) {
    return R::process(F::process(std::move(args)...));
  }
};

template <typename F, typename R>
struct regular_element_impl<F, R, filter_proc>
    : regular_element_impl_varargs<
          F, R, typename traits::filter_traits<F>::args_type> {};

template <typename R, typename... Ps>
struct regular_element_impl<branches<Ps...>, R, filter_proc>
    : helpers::requires_v<concept ::io_compatible_x<R, Ps...>()> {

  using f_arg_type = helpers::front_t<
      helpers::merge_all_t<typename traits::filter_traits<Ps>::args_type...>>;

  static constexpr auto process(f_arg_type arg) {
    return R::process(Ps::process(arg)...);
  }
};

template <typename F, typename R>
struct regular_element_impl<F, R, filter_rev_proc>
    : regular_element_impl<F, R, filter_proc> {
  using r_ret_type = typename traits::filter_traits<R>::ret_type;

  static constexpr auto reverse(r_ret_type arg) {
    return F::reverse(R::reverse(std::move(arg)));
  }
};

template <typename F, typename T> struct end_element_impl;

template <typename F> struct end_element_impl<F, filter_gen> {
  static constexpr auto process() { return F::process(); }
};

template <typename F, typename Args> struct end_element_impl_varargs;

template <typename F, template <typename...> typename List, typename... Args>
struct end_element_impl_varargs<F, List<Args...>>
    : helpers::requires_v<concept ::check_args<F, Args...>()> {
  static constexpr auto process(Args... args) {
    return F::process(std::move(args)...);
  }
};

template <typename F>
struct end_element_impl<F, filter_proc>
    : end_element_impl_varargs<F,
                               typename traits::filter_traits<F>::args_type> {};

template <typename F>
struct end_element_impl<F, filter_rev_proc> : end_element_impl<F, filter_proc> {
  using r_ret_type = typename traits::filter_traits<F>::ret_type;

  static constexpr auto reverse(r_ret_type arg) {
    return F::reverse(std::move(arg));
  }
};
} // namespace detail

template <typename F, typename R>
struct regular_element
    : detail::regular_element_impl<
          F, R, typename traits::filter_traits<F>::filter_type> {};

template <typename F>
struct end_element
    : detail::end_element_impl<F,
                               typename traits::filter_traits<F>::filter_type> {
};

template <typename F, typename R>
struct pipe_element_selector : regular_element<F, R> {};

template <typename F>
struct pipe_element_selector<F, helpers::nonsuch> : end_element<F> {};

template <typename F, typename R = helpers::nonsuch>
struct pipe_element : pipe_element_selector<F, R> {};

template <typename... Args> struct pipe;

template <typename F, typename... R>
struct pipe<F, R...> : pipe_element<F, pipe<R...>> {};

template <typename F> struct pipe<F> : pipe_element<F> {};
} // namespace pipet