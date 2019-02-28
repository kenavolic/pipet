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

#include <bitset>
#include <cstdint>
#include <iostream>

namespace pipet::extra {
constexpr unsigned bit_count(uint64_t n) {
  return (n ? (n & 0x1) + bit_count(n >> 1) : 0);
}

template <typename T, typename... Is> constexpr T bit_mask(Is... indices) {
  return ((T(1) << indices) | ...);
}

template <typename T> auto stringify(T n) {
  return "0b" + std::bitset<sizeof(T) * 8>{n}.to_string();
}
} // namespace pipet::extra