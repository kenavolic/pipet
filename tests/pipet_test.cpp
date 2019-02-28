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

#include "pipet/pipet.h"
#include "test_common.h"

using namespace pipet;
using namespace pipet::test;

TEST(pipet_test, main) {
  // test pipe building and running
  static_assert(
      pipet::pipe<f1_rev_proc_ct, f2_rev_proc_ct, f3_rev_proc_ct>::process(1) ==
          std::tuple<int, int>{2, 2},
      "[-][pipet_test] pipe running failed");

  // test pipe with data generator
  static_assert(pipet::pipe<fo_gen_ct, f1_rev_proc_ct, f2_rev_proc_ct,
                            f3_rev_proc_ct>::process() ==
                    std::tuple<int, int>{2, 2},
                "[-][pipet_test] pipe running failed");

  // test pipe with only none reversible filter
  static_assert(
      pipet::pipe<f1_proc_ct, f2_rev_proc_ct, f3_rev_proc_ct>::process(1) ==
          std::tuple<int, int>{2, 2},
      "[-][pipet_test] pipe running failed");

  // test 1 filter specific case
  static_assert(pipet::pipe<fo_gen_ct>::process() == 1,
                "[-][pipet_test] pipe running failed");
  static_assert(pipet::pipe<f1_rev_proc_ct>::process(1) == 1,
                "[-][pipet_test] pipe running failed");

  // test reversing at compile-time
  static_assert(
      pipet::pipe<f1_rev_proc_ct, f2_rev_proc_ct, f3_rev_proc_ct>::reverse(
          std::tuple<int, int>{2, 2}) == 1,
      "[-][pipet_test] pipe reversing failed");

  // test building pipe from empty one (use the power of generic typelist algo)
  using empty_pipe_t = pipet::pipe<>;
  using pipe_1_t = helpers::push_front_t<fo_gen_ct, empty_pipe_t>;
  using pipe_2_t = helpers::push_back_t<f1_rev_proc_ct, pipe_1_t>;

  static_assert(std::is_same_v<pipe_1_t, pipet::pipe<fo_gen_ct>>,
                "[-][pipet_test] pipe building failed");
  static_assert(
      std::is_same_v<pipe_2_t, pipet::pipe<fo_gen_ct, f1_rev_proc_ct>>,
      "[-][pipet_test] pipe building failed");

  // test buiding a pipe for run-time execution
  EXPECT_EQ((pipet::pipe<fo_gen_rt, f1_proc_rt>::process()), 1);
}

int pipet_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "pipet_test*";

  return RUN_ALL_TESTS();
}