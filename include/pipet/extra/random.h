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

#include <limits>
#include <type_traits>

#include "pipet/helpers/utils.h"

namespace pipet::extra {
namespace concept {
  template <typename T, T S, T A, T M> constexpr bool is_lcg_compatible() {
    return std::conjunction_v<
        std::is_integral<T>, std::is_unsigned<T>, helpers::is_non_zero<T, S>,
        helpers::is_non_zero<T, A>, helpers::is_non_zero<T, M>,
        helpers::is_unsigned_compatible<T, S, int64_t>>;
  }
} // namespace concept

template <typename T, T S, T A, T M> class mul_lcg {
  static_assert(concept ::is_lcg_compatible<T, S, A, M>(),
                "[-][pipet] invalid lcg params");
  T _q{M / A};
  T _r{M % A};

  constexpr auto seed(int64_t s) const {
    s = A * (s % _q) - _r * (s / _q);

    if (s < 0) {
      s += M;
    }

    return s;
  }

public:
  constexpr mul_lcg() {}

  constexpr T rand(std::size_t round) const {
    int64_t s = S;
    while (round--) {
      s = seed(s);
    }
    return static_cast<T>(s);
  }

  constexpr T rand(std::size_t round, T low, T high) const {
    return low + rand(round) % (high - low);
  }
};

template <typename T> using minstand_lcg = mul_lcg<T, 1u, 16807, 2147483647>;
} // namespace pipet::extra