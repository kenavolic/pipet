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

#include "pipet/filter.h"
#include "test_common.h"

using namespace pipet;
using namespace pipet::concept;
using namespace pipet::helpers;
using namespace pipet::test;
using namespace pipet::traits;

//-------------------------------------
// Dynamic Tests

TEST(filter_test, main) {
  // test trait
  static_assert(
      std::is_same_v<int, typename filter_traits<f1_rev_proc_ct>::ret_type>,
      "[-][filter_test] trait instantiation failed");
  static_assert(
      std::is_same_v<int, typename filter_traits<f1_rev_proc_ct>::arg_type>,
      "[-][filter_test] trait instantiation failed");
  static_assert(std::is_same_v<filter_gen,
                               typename filter_traits<fo_gen_ct>::filter_type>,
                "[-][filter_test] trait instantiation failed");
  static_assert(
      std::is_same_v<filter_rev_proc,
                     typename filter_traits<f1_rev_proc_ct>::filter_type>,
      "[-][filter_test] trait instantiation failed");
  static_assert(std::is_same_v<filter_proc,
                               typename filter_traits<f1_proc_ct>::filter_type>,
                "[-][filter_test] trait instantiation failed");

  // test io checker
  static_assert(!io_checker<f1_proc_ct, f3_rev_proc_ct>::value,
                "[-][filter_test] io checker failed");
  static_assert(io_checker<f1_proc_ct, f2_rev_proc_ct>::value,
                "[-][filter_test] io checker failed");

  // no runtime test
  EXPECT_TRUE(true);
}

int filter_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "filter_test*";

  return RUN_ALL_TESTS();
}