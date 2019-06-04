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

#include "pipet/extra/bit.h"

#include "gtest/gtest.h"

using namespace pipet::extra;

TEST(bit_test, main) {
  // test bit_count
  static_assert(bit_count(0b1) == 1, "[-][bit_test] bit_count failed");
  static_assert(bit_count(0b0) == 0, "[-][bit_test] bit_count failed");
  static_assert(bit_count(0b11111111) == 8, "[-][bit_test] bit_count failed");
  static_assert(
      bit_count(
          0b1000000000000000000000000000000000000000000000000000000000000001) ==
          2,
      "[-][bit_test] bit_count failed");
  static_assert(bit_count(33685634) == 4, "[-][bit_test] bit_count failed");

  // test bit_mask
  static_assert(bit_mask<unsigned>(0, 1, 2, 3) == 0b1111,
                "[-][bit_test] bit_mask failed");
  static_assert(bit_mask<unsigned>(0, 2, 4) == 0b10101,
                "[-][bit_test] bit_mask failed");

  // runtime tests
  EXPECT_EQ(bit_count(0b0), 0);
  EXPECT_EQ(bit_count(0b1), 1);
  EXPECT_EQ(bit_count(0b11111111), 8);

  EXPECT_EQ(stringify(0b0u), "0b00000000000000000000000000000000");
  EXPECT_EQ(stringify(0b10u), "0b00000000000000000000000000000010");
  EXPECT_EQ(stringify(0b1011u), "0b00000000000000000000000000001011");
}

int bit_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "bit_test*";

  return RUN_ALL_TESTS();
}