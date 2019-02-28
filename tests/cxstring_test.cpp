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

#include "pipet/extra/cxstring.h"

using namespace pipet::extra;

TEST(cxstring_test, main) {
  constexpr auto ctstr = make_cxstring("test");
  static_assert(ctstr.size() == 5, "[-][cxstring_test] size failed");
  static_assert(ctstr[0] == 't', "[-][cxstring_test] [] failed");
  static_assert(ctstr[3] == 't', "[-][cxstring_test] [] failed");
  static_assert(ctstr[4] == '\0', "[-][cxstring_test] [] failed");

  auto rtstr = make_cxstring("test2");

  EXPECT_EQ(rtstr.size(), 6);
  EXPECT_EQ(rtstr[0], 't');
  EXPECT_EQ(rtstr[4], '2');
  EXPECT_EQ(rtstr[5], '\0');

  std::string stdstr = rtstr;
  EXPECT_TRUE(stdstr == "test2");
}

int cxstring_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "cxstring_test*";

  return RUN_ALL_TESTS();
}