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

#include <cstdint>
#include <limits>
#include <type_traits>

namespace pipet::helpers {
struct nonsuch {
  nonsuch() = delete;
  ~nonsuch() = delete;
  nonsuch(const nonsuch &) = delete;
  void operator=(const nonsuch &) = delete;
};

template <bool B> struct requires_v {
  static_assert(B, "[-][pipet] requirement not met");
};

template <typename T, T val, typename = std::integral_constant<T, val>>
struct is_non_zero : std::true_type {};

template <typename T, T val>
struct is_non_zero<T, val, std::integral_constant<T, 0>> : std::false_type {};

template <typename S, S val, typename T> struct is_unsigned_compatible {
  static_assert(std::is_unsigned_v<S> && std::is_integral_v<T>,
                "[-][pipet] bad usage");
  static constexpr bool value = (val <= (std::numeric_limits<T>::max)());
};
} // namespace pipet::helpers