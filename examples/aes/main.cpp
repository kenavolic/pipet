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

#include <iostream>
#include <string>

#include "pipet/extra/cxstring.h"

#include "aes.h"

using namespace pipet::extra;
using namespace aes;

namespace {
template <typename Str, std::size_t... Is>
constexpr auto cxstr2arr(Str const &str, std::index_sequence<Is...>) {
  return std::array<uint8_t, sizeof...(Is)>{static_cast<uint8_t>(str[Is])...};
}

template <std::size_t N>
constexpr auto aes_ecb_cipher_str(serial_key const &k, cxstring<N> const &str) {
  return aes_ecb_cipher(k, cxstr2arr(str, std::make_index_sequence<N>()));
}
} // namespace

int main() {
  constexpr serial_key kraw = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                               0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
  constexpr auto k = exp_key<10>(kraw);

  static_assert(equals(k.serial_at(0), kraw), "[-][aes] bad key computation");
  static_assert(
      equals(k.serial_at(1),
             serial_key{0xa0, 0xfa, 0xfe, 0x17, 0x88, 0x54, 0x2c, 0xb1, 0x23,
                        0xa3, 0x39, 0x39, 0x2a, 0x6c, 0x76, 0x05}),
      "[aes] bad key computation");
  static_assert(
      equals(k.serial_at(10),
             serial_key{0xd0, 0x14, 0xf9, 0xa8, 0xc9, 0xee, 0x25, 0x89, 0xe1,
                        0x3f, 0x0c, 0xc8, 0xb6, 0x63, 0x0c, 0xa6}),
      "[aes] bad key computation");

  constexpr auto s = state{{0x32, 0x43, 0xf6, 0xa8},
                           {0x88, 0x5a, 0x30, 0x8d},
                           {0x31, 0x31, 0x98, 0xa2},
                           {0xe0, 0x37, 0x07, 0x34}};

  // test first step
  constexpr auto s1 = inject_key(s, k.get_at(0));
  static_assert(equals(s1.c0, {0x19, 0x3d, 0xe3, 0xbe}),
                "[-][aes] bad rkv1 computation");

  // test filters
  constexpr auto s2 = subbyte_filter::process(s1);
  static_assert(equals(s2.c0, {0xd4, 0x27, 0x11, 0xae}),
                "[-][aes] bad subbyte_filter computation");
  static_assert(equals(s2.c1, {0xe0, 0xbf, 0x98, 0xf1}),
                "[-][aes] bad subbyte_filter computation");
  static_assert(equals(s2.c2, {0xb8, 0xb4, 0x5d, 0xe5}),
                "[-][aes] bad subbyte_filter computation");
  static_assert(equals(s2.c3, {0x1e, 0x41, 0x52, 0x30}),
                "[-][aes] bad subbyte_filter computation");

  constexpr auto s3 = shiftrow_filter::process(s2);
  static_assert(equals(s3.c0, {0xd4, 0xbf, 0x5d, 0x30}),
                "[-][aes] bad shiftrow_filter computation");
  static_assert(equals(s3.c1, {0xe0, 0xb4, 0x52, 0xae}),
                "[-][aes] bad shiftrow_filter computation");
  static_assert(equals(s3.c2, {0xb8, 0x41, 0x11, 0xf1}),
                "[-][aes] bad shiftrow_filter computation");
  static_assert(equals(s3.c3, {0x1e, 0x27, 0x98, 0xe5}),
                "[-][aes] bad shiftrow_filter computation");

  constexpr auto s4 = mixcol_filter::process(s3);
  static_assert(equals(s4.c0, {0x04, 0x66, 0x81, 0xe5}),
                "[-][aes] bad mixcol_filter computation");
  static_assert(equals(s4.c1, {0xe0, 0xcb, 0x19, 0x9a}),
                "[-][aes] bad mixcol_filter computation");
  static_assert(equals(s4.c2, {0x48, 0xf8, 0xd3, 0x7a}),
                "[-][aes] bad mixcol_filter computation");
  static_assert(equals(s4.c3, {0x28, 0x06, 0x26, 0x4c}),
                "[-][aes] bad mixcol_filter computation");

  // test aes
  constexpr auto aes_encryptor = aes_cipher{kraw};
  constexpr auto cipher_text = aes_encryptor.cipher(s);

  static_assert(equals(cipher_text.c0, {0x39, 0x25, 0x84, 0x1d}),
                "[-][aes] bad aes encryption");
  static_assert(equals(cipher_text.c1, {0x02, 0xdc, 0x09, 0xfb}),
                "[-][aes] bad aes encryption");
  static_assert(equals(cipher_text.c2, {0xdc, 0x11, 0x85, 0x97}),
                "[-][aes] bad aes encryption");
  static_assert(equals(cipher_text.c3, {0x19, 0x6a, 0x0b, 0x32}),
                "[-][aes] bad aes encryption");

  constexpr auto plain_text = aes_encryptor.decipher(cipher_text);
  static_assert(equals(plain_text.c0, {0x32, 0x43, 0xf6, 0xa8}),
                "[-][aes] bad aes decryption");
  static_assert(equals(plain_text.c1, {0x88, 0x5a, 0x30, 0x8d}),
                "[-][aes] bad aes decryption");
  static_assert(equals(plain_text.c2, {0x31, 0x31, 0x98, 0xa2}),
                "[-][aes] bad aes decryption");
  static_assert(equals(plain_text.c3, {0xe0, 0x37, 0x07, 0x34}),
                "[-][aes] bad aes decryption");

  // test aes with variable block size
  constexpr auto cipher_text_var_small =
      aes_ecb_cipher_str(kraw, make_cxstring("small str"));
  static_assert(cipher_text_var_small[0].c0[0] != 0xca,
                "[-][aes] bad ecb ciphering");

  constexpr auto cipher_text_var_large = aes_ecb_cipher_str(
      kraw, make_cxstring("this is a test string longer than 128 bits"));

  // test runtime
  auto plain_text_var_small =
      aes_ecb_decipher<std::vector<uint8_t>>(kraw, cipher_text_var_small);
  auto plain_text_var_large =
      aes_ecb_decipher<std::vector<uint8_t>>(kraw, cipher_text_var_large);

  std::cout << "[--- aes deciphering ---]" << std::endl;
  std::cout << "small blob (size/data) = " << plain_text_var_small.size()
            << " / "
            << std::string(
                   reinterpret_cast<char *>(plain_text_var_small.data()))
            << std::endl;
  std::cout << "large blob (size/data) = " << plain_text_var_large.size()
            << " / "
            << std::string(
                   reinterpret_cast<char *>(plain_text_var_large.data()))
            << std::endl;
  return 0;
}