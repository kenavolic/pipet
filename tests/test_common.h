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

#include <memory>
#include <tuple>

namespace pipet::test {
struct fo_gen_ct {
  static constexpr auto process() { return int{1}; }
};

struct fo_gen_rt {
  static auto process() { return std::make_unique<int>(1); }
};

struct f1_rev_proc_ct {
  static constexpr auto process(int a) { return a; }

  static constexpr auto reverse(int a) { return a; }
};

struct f1_proc_ct {
  static constexpr auto process(int a) { return a; }
};

struct f1_proc_rt {
  static auto process(std::unique_ptr<int> a) { return *a; }
};

struct f2_rev_proc_ct {
  static constexpr auto process(int a) { return std::make_tuple(a, a); }

  static constexpr auto reverse(std::tuple<int, int> t) {
    return std::get<0>(std::move(t));
  }
};

struct f3_rev_proc_ct {
  static constexpr auto process(std::tuple<int, int> t) {
    return std::make_tuple(std::get<0>(t) + 1, std::get<1>(t) + 1);
  }

  static constexpr auto reverse(std::tuple<int, int> t) {
    return std::make_tuple(std::get<0>(t) - 1, std::get<1>(t) - 1);
  }
};

// compute a*a
struct f_square_ct {
  static constexpr auto process(int a) { return a * a; }
};

// compute a*a*a
struct f_cube_ct {
  static constexpr auto process(int a) { return a * a * a; }
};

// add 3 different inputs
struct f_add3_ct {
  static constexpr auto process(int a, int b, int c) { return a + b + c; }
};
} // namespace pipet::test