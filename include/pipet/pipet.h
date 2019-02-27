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

#include <type_traits>

#include "filter.h"
#include "helpers/utils.h"

namespace pipet {
namespace detail {
template <typename F, typename R, typename T> struct regular_element_impl;

template <typename F, typename R>
struct regular_element_impl<F, R, filter_gen> {
  static constexpr auto process() { return R::process(F::process()); }
};

template <typename F, typename R>
struct regular_element_impl<F, R, filter_proc> {
  static_assert(concept ::io_compatible<F, R>(), "[-][pipet] invalid io");

  using f_arg_type = typename traits::filter_traits<F>::arg_type;

  static constexpr auto process(f_arg_type arg) {
    return R::process(F::process(std::move(arg)));
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

template <typename F> struct end_element_impl<F, filter_proc> {
  using f_arg_type = typename traits::filter_traits<F>::arg_type;

  static constexpr auto process(f_arg_type arg) {
    return F::process(std::move(arg));
  }
};

template <typename F>
struct end_element_impl<F, filter_rev_proc> : end_element_impl<F, filter_proc> {
  using r_ret_type = typename traits::filter_traits<F>::ret_type;

  static constexpr auto reverse(r_ret_type arg) {
    return F::reverse(std::move(arg));
  }
};
} // namespace detail

///
/// @brief Pipe basic block
///
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

///
/// @brief Compile-time constructible pipe
///
template <typename... Args> struct pipe;

template <typename F, typename... R>
struct pipe<F, R...> : pipe_element<F, pipe<R...>> {};

template <typename F> struct pipe<F> : pipe_element<F> {};
} // namespace pipet