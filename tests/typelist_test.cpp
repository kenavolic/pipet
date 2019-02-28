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

#include "gtest/gtest.h"

#include <tuple>
#include <type_traits>

#include "pipet/helpers/typelist.h"

using namespace pipet::helpers;

//-------------------------------------
// Dynamic Tests

TEST(typelist_test, main) {
  // is empty
  static_assert(is_empty_v<typelist<>>, "[-][typelist_test] is_empty failed");
  static_assert(is_empty_v<std::tuple<>>, "[-][typelist_test] is_empty failed");
  static_assert(!is_empty_v<typelist<int>>,
                "[-][typelist_test] is_empty failed");

  // size
  static_assert(0 == size_v<typelist<>>, "[-][typelist_test] size failed");
  static_assert(1 == size_v<typelist<int>>, "[-][typelist_test] size failed");
  static_assert(2 == size_v<typelist<int, double>>,
                "[-][typelist_test] size failed");

  // front
  static_assert(std::is_same_v<int, front_t<typelist<int, double>>>,
                "[-][typelist_test] front failed");
  static_assert(std::is_same_v<int, front_t<std::tuple<int>>>,
                "[-][typelist_testtest] front failed");

  // pop front
  static_assert(
      std::is_same_v<pop_front_t<typelist<int, double>>, typelist<double>>,
      "[-][typelist_test] popfront failed");
  static_assert(std::is_same_v<pop_front_t<typelist<int>>, typelist<>>,
                "[-][typelist_test] popfront failed");

  // push front
  static_assert(std::is_same_v<push_front_t<int, typelist<double>>,
                               typelist<int, double>>,
                "[-][typelist_test] pushfront failed");
  static_assert(std::is_same_v<push_front_t<int, typelist<>>, typelist<int>>,
                "[-][typelist_test] pushfront failed");

  // push back
  static_assert(std::is_same_v<push_back_t<int, typelist<>>, typelist<int>>,
                "[-][typelist_test] pushback failed");
  static_assert(
      std::is_same_v<push_back_t<double, typelist<int>>, typelist<int, double>>,
      "[-][typelist_test] pushback failed");

  // pop back
  static_assert(
      std::is_same_v<pop_back_t<typelist<int, double>>, typelist<int>>,
      "[-][typelist_test] popback failed");
  static_assert(std::is_same_v<pop_back_t<typelist<int>>, typelist<>>,
                "[-][typelist_test] popback failed");

  // has
  static_assert(has_v<int, typelist<double, int>>,
                "[-][typelist_test] has failed");
  static_assert(has_v<int, typelist<int, double>>,
                "[-][typelist_test] has failed");
  static_assert(!has_v<int, typelist<float, double>>,
                "[-][typelist_test] has failed");

  // remove dup
  static_assert(std::is_same_v<remove_dup_t<typelist<int, int>>, typelist<int>>,
                "[-][typelist_test] remove_dup failed");
  static_assert(std::is_same_v<remove_dup_t<typelist<int, double>>,
                               typelist<int, double>>,
                "[-][typelist_test] remove_dup failed");

  // rebind
  static_assert(std::is_same_v<rebind_t<std::tuple, typelist<int, int>>,
                               std::tuple<int, int>>,
                "[-][typelist_test] rebind failed");

  // no runtime test
  EXPECT_TRUE(true);
}

int typelist_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "typelist_test*";

  return RUN_ALL_TESTS();
}