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

#include "pipet/helpers/utils.h"

using namespace pipet::helpers;

TEST(utils_test, main) {
  // test is_non_zero
  static_assert(is_non_zero<int, 1>::value,
                "[-][utils_test] is_non_zero failed");
  static_assert(is_non_zero<unsigned, 1>::value,
                "[-][utils_test] is_non_zero failed");
  static_assert(!is_non_zero<unsigned, 0>::value,
                "[-][utils_test] is_non_zero failed");

  // test is_unsigned_compatible
  static_assert(is_unsigned_compatible<unsigned, 0, int32_t>::value,
                "[-][utils_test] is_unsigned_compatible failed");
  static_assert(!is_unsigned_compatible<unsigned, 1024, int8_t>::value,
                "[-][utils_test] is_unsigned_compatible failed");
}

int utils_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "utils_test*";

  return RUN_ALL_TESTS();
}