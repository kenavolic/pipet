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

#include "pipet/helpers/reflect.h"

#include "gtest/gtest.h"

using namespace pipet::helpers;

//-------------------------------------
// Utility

namespace {
struct foo {
  int test(int, double);
};

struct bar {
  int test(int);
};

int f(int, double);
void g();

template <typename T, typename Ret, typename... Args>
using has_test_t = std::integral_constant<Ret (T::*)(Args...), &T::test>;

template <typename T, typename Ret, typename... Args>
constexpr bool has_test1_v = is_detected_v<has_test_t, T, Ret, Args...>;

constexpr auto has_first =
    is_valid([](auto x) -> decltype((void)value_t(x).first) {});
} // namespace

TEST(reflect_test, main) {
  // is_detected static tests
  static_assert(has_test1_v<foo, int, int, double>,
                "[-][reflect_test] is_detected failed");
  // static_assert(!has_test_v<bar, int, int, double>, "[-][reflect_test] static
  // is_detected failed");

  // is_valid static tests
  static_assert(!has_first(type<std::tuple<int, int>>),
                "[-][reflect_test] is_valid failed");
  static_assert(has_first(type<std::pair<int, int>>),
                "[-][reflect_test] is_valid failed");

  constexpr auto has_test2 = is_valid(
      [](auto x, auto &&... args) -> decltype((void)value_t(x).test(args...)) {
      });
  static_assert(has_test2(type<foo>, int{}, double{}),
                "[-][reflect_test] is_valid failed");
  static_assert(!has_test2(type<bar>, int{}, double{}),
                "[-][reflect_test] is_valid failed");

  // function ret and args
  static_assert(std::is_same_v<int, decltype(function_ret(f))>,
                "[-][reflect_test] function_ret failed");
  static_assert(std::is_same_v<void, decltype(function_ret(g))>,
                "[-][reflect_test] function_ret failed");
  static_assert(
      std::is_same_v<typelist<int, double>, decltype(function_args(f))>,
      "[-][reflect_test] function_args failed");
  static_assert(std::is_same_v<typelist<>, decltype(function_args(g))>,
                "[-][reflect_test] function_args failed");

  // no dynamic test
  EXPECT_TRUE(true);
}

int reflect_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "reflect_test*";

  return RUN_ALL_TESTS();
}