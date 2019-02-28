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

#include <cstdint>
#include <iostream>
#include <type_traits>

#include "pipet/extra/cxstring.h"
#include "pipet/extra/random.h"
#include "pipet/pipet.h"

using namespace pipet;
using namespace pipet::extra;
using namespace pipet::helpers;

//
// Basic example used to hide strings from automated tools
// (like linux strings utility etc) in order to add another
// layer of (very light!) protection to your software
//

namespace {
template <std::size_t N> struct fixed_xor_filter {
  // internal fixed key
  static constexpr uint8_t key[10] = {0xef, 0x1a, 0xb3, 0x4f, 0xda,
                                      0x32, 0x16, 0x75, 0x14, 0x56};

  // internal data type
  using data_type = cxstring<N>;

  // processing
  template <std::size_t... Is>
  static constexpr auto cxstring_unpack(data_type str,
                                        std::index_sequence<Is...>) {
    return data_type({static_cast<char>(str[Is] ^ key[Is % sizeof(key)])...});
  }

  static constexpr auto process(data_type str) {
    return cxstring_unpack(std::move(str), std::make_index_sequence<N>());
  }

  static auto reverse(data_type str) { return process(std::move(str)); }
};

template <std::size_t N> struct variable_xor_filter {
  // internal data type
  using random_gen = minstand_lcg<uint32_t>;
  using data_type = cxstring<N>;

  // processing
  template <std::size_t... Is>
  static constexpr auto cxstring_unpack(data_type str,
                                        std::index_sequence<Is...>) {
    // For the sake of simplicity, all strings with same size will have same
    // offset in generator round thus same obfuscation key. Other properties
    // such as compile-time string hashes would be more appropriate here.
    return data_type({static_cast<char>(
        str[Is] ^ random_gen{}.rand(Is + sizeof...(Is), 1, 255))...});
  }

  static constexpr auto process(data_type str) {
    return cxstring_unpack(std::move(str), std::make_index_sequence<N>());
  }

  static auto reverse(data_type str) { return process(std::move(str)); }
};

template <std::size_t N> struct inverter_filter {
  // internal data type
  using data_type = cxstring<N>;

  // processing
  template <std::size_t... Is>
  static constexpr auto cxstring_unpack(data_type str,
                                        std::index_sequence<Is...>) {
    return data_type{{str[sizeof...(Is) - Is - 1]...}};
  }

  static constexpr auto process(data_type str) {
    return cxstring_unpack(std::move(str), std::make_index_sequence<N>());
  }

  static auto reverse(data_type str) { return process(std::move(str)); }
};

template <std::size_t N> constexpr bool is_ss_compatible() {
  return (N <= std::extent_v<decltype(fixed_xor_filter<N>::key)>);
}

template <std::size_t N, bool B = is_ss_compatible<N>()> struct obfuscator;

template <std::size_t N>
struct obfuscator<N, true> : pipe<fixed_xor_filter<N>> {};

template <std::size_t N>
struct obfuscator<N, false>
    : push_back_t<inverter_filter<N>, push_back_t<variable_xor_filter<N>,
                                                  pipe<fixed_xor_filter<N>>>> {
};

template <std::size_t N> constexpr auto obfuscate(cxstring<N> str) {
  return obfuscator<N>::process(str);
}

template <std::size_t N> auto deobfuscate(cxstring<N> str) {
  return std::string(obfuscator<N>::reverse(str));
}
} // namespace

int main() {

  // volatile is needed here to avoid some optimization
  constexpr volatile auto cipher1 = obfuscate(make_cxstring("copyright"));
  constexpr volatile auto cipher2 = obfuscate(make_cxstring("enter password"));
  constexpr volatile auto cipher3 = obfuscate(make_cxstring("bad password"));

  std::cout << "[--- plain strings ---]" << std::endl;
  std::cout << "plain1 = "
            << deobfuscate(
                   *(const_cast<std::remove_volatile_t<decltype(cipher1)> *>(
                       &cipher1)))
            << std::endl;
  std::cout << "plain2 = "
            << deobfuscate(
                   *(const_cast<std::remove_volatile_t<decltype(cipher2)> *>(
                       &cipher2)))
            << std::endl;
  std::cout << "plain3 = "
            << deobfuscate(
                   *(const_cast<std::remove_volatile_t<decltype(cipher3)> *>(
                       &cipher3)))
            << std::endl;

  return 0;
}