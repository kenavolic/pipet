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

#include <array>
#include <cstdint>
#include <iostream>

#include "pipet/extra/bit.h"
#include "pipet/extra/random.h"
#include "pipet/pipet.h"

using namespace pipet;
using namespace pipet::helpers;
using namespace pipet::extra;

//
// Basic example used to generate a mask with n bits set
//

namespace {
template <std::size_t N> using num_sequence_t = std::array<std::size_t, N>;

template <std::size_t N> struct n_lcg_filter {
  using rand_type = minstand_lcg<uint32_t>;

  static constexpr auto process(uint32_t offset) {
    rand_type generator{};
    auto counter{N};
    num_sequence_t<N> seq{};

    while (counter--) {
      // in real world, collisions should be checked
      seq[counter] =
          generator.rand(counter + offset, 0, sizeof(std::size_t) * 8);
    }

    return seq;
  }
};

template <std::size_t N> struct n_bit_mask_filter {
  template <std::size_t... Is>
  static constexpr auto bit_mask_unpack(num_sequence_t<N> seq,
                                        std::index_sequence<Is...>) {
    return bit_mask<std::size_t>(seq[Is]...);
  }

  static constexpr auto process(num_sequence_t<N> seq) {
    return bit_mask_unpack(std::move(seq), std::make_index_sequence<N>());
  }
};
} // namespace

int main() {
  constexpr std::size_t mask_size = 4;
  using mask_generator_t =
      pipe<n_lcg_filter<mask_size>, n_bit_mask_filter<mask_size>>;

  constexpr auto mask1 = mask_generator_t::process(0 * mask_size);
  constexpr auto mask2 = mask_generator_t::process(1 * mask_size);
  constexpr auto mask3 = mask_generator_t::process(2 * mask_size);

  static_assert(bit_count(mask1) == 4, "[-][randmask] bad mask");
  static_assert(bit_count(mask2) == 4, "[-][randmask] bad mask");
  static_assert(bit_count(mask3) == 4, "[-][randmask] bad mask");

  static_assert(mask1 != mask2, "[-][randmask] bad random mask");
  static_assert(mask1 != mask3, "[-][randmask] bad random mask");
  static_assert(mask2 != mask3, "[-][randmask] bad random mask");

  std::cout << "[--- mask generation ---]" << std::endl;
  std::cout << "mask1 = " << stringify(mask1) << std::endl;
  std::cout << "mask2 = " << stringify(mask2) << std::endl;
  std::cout << "mask3 = " << stringify(mask3) << std::endl;

  return 0;
}