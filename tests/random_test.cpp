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

#include "pipet/extra/random.h"

using namespace pipet::extra;

namespace {
// recursive implementation of lcg sensitive to overflow
constexpr uint64_t unsigned_lcg(uint64_t round) {
  return (round ? (unsigned_lcg(round - 1) * 16807) % 2147483647 : 1u);
}
} // namespace

TEST(random_test, main) {
  static_assert(minstand_lcg<uint32_t>{}.rand(0) == unsigned_lcg(0),
                "[-][random_test] rand failed");
  static_assert(minstand_lcg<uint32_t>{}.rand(1) == unsigned_lcg(1),
                "[-][random_test] rand failed");
  static_assert(minstand_lcg<uint32_t>{}.rand(2, 0, 100) ==
                    (unsigned_lcg(2) % 100),
                "[-][random_test] rand failed");

  // runtime check
  minstand_lcg<uint32_t> gen;
  EXPECT_EQ(gen.rand(2), unsigned_lcg(2));
  EXPECT_EQ(gen.rand(3), unsigned_lcg(3));
  EXPECT_EQ(gen.rand(256), unsigned_lcg(256));
  EXPECT_EQ(gen.rand(256, 5, 128), 5 + unsigned_lcg(256) % (128 - 5));
}

int random_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "random_test*";

  return RUN_ALL_TESTS();
}