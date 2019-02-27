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

#include <array>
#include <cassert>
#include <tuple>
#include <type_traits>
#include <vector>

#include "pipet/pipet.h"
#include "tables.h"

//
// Aes 128 naive implementation
//

namespace aes {
// base type
using serial_key = std::array<uint8_t, 16>;
using serial_state = std::array<uint8_t, 16>;

// Note that word is implemented as an array to stick
// with the original paper concept for educational purpose
// (proper implementation would use 32-bits integer arithmetic
//  with adjustment for endianness)
using word = std::array<uint8_t, 4>;

struct mat4x4 {
  word c0;
  word c1;
  word c2;
  word c3;
};

using state = mat4x4;
using key = mat4x4;

namespace detail {
// aes implementation details

constexpr auto rol(word const &w) { return word{w[1], w[2], w[3], w[0]}; }

constexpr uint8_t gmul(uint8_t a, uint8_t b) {
  if (!a || !b) {
    return 0;
  } else {
    uint16_t res = l_table[a >> 4][a & 0xF] + l_table[b >> 4][b & 0xF];

    if (res > 0xFF)
      res -= 0xFF;

    return e_table[res >> 4][res & 0xF];
  }
}

constexpr auto mixcol(word const &w) {
  return word{static_cast<uint8_t>(gmul(w[0], 2) ^ gmul(w[1], 3) ^
                                   gmul(w[2], 1) ^ gmul(w[3], 1)),
              static_cast<uint8_t>(gmul(w[0], 1) ^ gmul(w[1], 2) ^
                                   gmul(w[2], 3) ^ gmul(w[3], 1)),
              static_cast<uint8_t>(gmul(w[0], 1) ^ gmul(w[1], 1) ^
                                   gmul(w[2], 2) ^ gmul(w[3], 3)),
              static_cast<uint8_t>(gmul(w[0], 3) ^ gmul(w[1], 1) ^
                                   gmul(w[2], 1) ^ gmul(w[3], 2))};
}

constexpr auto mixcol_inv(word const &w) {
  return word{static_cast<uint8_t>(gmul(w[0], 0x0e) ^ gmul(w[1], 0x0b) ^
                                   gmul(w[2], 0x0d) ^ gmul(w[3], 0x09)),
              static_cast<uint8_t>(gmul(w[0], 0x09) ^ gmul(w[1], 0x0e) ^
                                   gmul(w[2], 0x0b) ^ gmul(w[3], 0x0d)),
              static_cast<uint8_t>(gmul(w[0], 0x0d) ^ gmul(w[1], 0x09) ^
                                   gmul(w[2], 0x0e) ^ gmul(w[3], 0x0b)),
              static_cast<uint8_t>(gmul(w[0], 0x0b) ^ gmul(w[1], 0x0d) ^
                                   gmul(w[2], 0x09) ^ gmul(w[3], 0x0e))};
}

constexpr auto ssub(word const &w) {
  return word{
      sbox_table[w[0] >> 4][w[0] & 0xF], sbox_table[w[1] >> 4][w[1] & 0xF],
      sbox_table[w[2] >> 4][w[2] & 0xF], sbox_table[w[3] >> 4][w[3] & 0xF]};
}

constexpr auto ssub_inv(word const &w) {
  return word{sbox_inv_table[w[0] >> 4][w[0] & 0xF],
              sbox_inv_table[w[1] >> 4][w[1] & 0xF],
              sbox_inv_table[w[2] >> 4][w[2] & 0xF],
              sbox_inv_table[w[3] >> 4][w[3] & 0xF]};
}

constexpr auto rcon(word const &w, std::size_t r) {
  return word{static_cast<uint8_t>(w[0] ^ rcon_table[r - 1]), w[1], w[2], w[3]};
}

// g expansion function
constexpr auto g(word const &w, std::size_t r) { return rcon(ssub(rol(w)), r); }

// helpers

template <typename Array, std::size_t... Is>
constexpr auto xor_array(Array const &rhs, Array const &lhs,
                         std::index_sequence<Is...>) {
  return Array{
      static_cast<std::decay_t<decltype(rhs[Is])>>(rhs[Is] ^ lhs[Is])...};
}

template <typename Array, std::size_t... Is>
constexpr auto equal_array(Array const &rhs, Array const &lhs,
                           std::index_sequence<Is...>) {
  return ((rhs[Is] == lhs[Is]) && ...);
}

constexpr auto serialize_key(word const &w0, word const &w1, word const &w2,
                             word const &w3) {
  return serial_key{w0[0], w0[1], w0[2], w0[3], w1[0], w1[1], w1[2], w1[3],
                    w2[0], w2[1], w2[2], w2[3], w3[0], w3[1], w3[2], w3[3]};
}

// make state from array view (should use c++20 span here)
constexpr auto parse_state(uint8_t const *view) {
  return state{
      {view[0], view[1], view[2], view[3]},
      {view[4], view[5], view[6], view[7]},
      {view[8], view[9], view[10], view[11]},
      {view[12], view[13], view[14], view[15]},
  };
}

template <typename Container> void push_word(Container &c, const word &s) {
  for (unsigned i = 0; i < 4; ++i) {
    c.emplace_back(s[i]);
  }
}

template <typename Container> void extend(Container &c, const state &s) {
  push_word(c, s.c0);
  push_word(c, s.c1);
  push_word(c, s.c2);
  push_word(c, s.c3);
}

template <typename Container> decltype(auto) remove_pad(Container &c) {
  while (c.back() == 0) {
    c.pop_back();
  }

  assert(c.back() == 1 && "[-][aes] bad padding");
  c.pop_back();

  return c;
}
} // namespace detail

// Key expansion

template <typename T, std::size_t N>
constexpr auto operator^(std::array<T, N> const &rhs,
                         std::array<T, N> const &lhs) {
  return detail::xor_array(rhs, lhs, std::make_index_sequence<N>{});
}

constexpr auto operator^(mat4x4 const &rhs, mat4x4 const &lhs) {
  return mat4x4{rhs.c0 ^ lhs.c0, rhs.c1 ^ lhs.c1, rhs.c2 ^ lhs.c2,
                rhs.c3 ^ lhs.c3};
}

template <typename T, std::size_t N>
constexpr bool equals(std::array<T, N> const &rhs,
                      std::array<T, N> const &lhs) {
  return detail::equal_array(rhs, lhs, std::make_index_sequence<N>{});
}

// expanded key
template <std::size_t N> struct exp_key : exp_key<N - 1> {
  constexpr exp_key(serial_key key)
      : exp_key<N - 1>(key), k{{exp_key<N - 1>::k.c0 ^
                                detail::g(exp_key<N - 1>::k.c3, N)},
                               {k.c0 ^ exp_key<N - 1>::k.c1},
                               {k.c1 ^ exp_key<N - 1>::k.c2},
                               {k.c2 ^ exp_key<N - 1>::k.c3}} {}

  constexpr auto serial_at(std::size_t i) const {
    return ((i == N) ? detail::serialize_key(k.c0, k.c1, k.c2, k.c3)
                     : exp_key<N - 1>::serial_at(i));
  }

  constexpr auto get_at(std::size_t i) const {
    return ((i == N) ? k : exp_key<N - 1>::get_at(i));
  }

  key const k;
};

template <> struct exp_key<0> {
  constexpr exp_key(serial_key const &k)
      : k{{k[0], k[1], k[2], k[3]},
          {k[4], k[5], k[6], k[7]},
          {k[8], k[9], k[10], k[11]},
          {k[12], k[13], k[14], k[15]}} {}

  constexpr auto serial_at(std::size_t i) const {
    return detail::serialize_key(k.c0, k.c1, k.c2, k.c3);
  }

  constexpr auto get_at(std::size_t i) const { return k; }

  key const k;
};

// Base state passed from filter to filter

// key injection
constexpr auto inject_key(state const &s, key const &rk) {
  return state{
      s.c0 ^ rk.c0,
      s.c1 ^ rk.c1,
      s.c2 ^ rk.c2,
      s.c3 ^ rk.c3,
  };
}

// Filters
struct subbyte_filter {
  static constexpr auto process(state const &s) {
    return state{
        detail::ssub(s.c0),
        detail::ssub(s.c1),
        detail::ssub(s.c2),
        detail::ssub(s.c3),
    };
  }

  static constexpr auto reverse(state const &s) {
    return state{
        detail::ssub_inv(s.c0),
        detail::ssub_inv(s.c1),
        detail::ssub_inv(s.c2),
        detail::ssub_inv(s.c3),
    };
  }
};

struct shiftrow_filter {
  static constexpr auto process(state const &s) {
    return state{
        {s.c0[0], s.c1[1], s.c2[2], s.c3[3]},
        {s.c1[0], s.c2[1], s.c3[2], s.c0[3]},
        {s.c2[0], s.c3[1], s.c0[2], s.c1[3]},
        {s.c3[0], s.c0[1], s.c1[2], s.c2[3]},
    };
  }

  static constexpr auto reverse(state const &s) {
    return state{
        {s.c0[0], s.c3[1], s.c2[2], s.c1[3]},
        {s.c1[0], s.c0[1], s.c3[2], s.c2[3]},
        {s.c2[0], s.c1[1], s.c0[2], s.c3[3]},
        {s.c3[0], s.c2[1], s.c1[2], s.c0[3]},
    };
  }
};

struct mixcol_filter {
  static constexpr auto process(state const &s) {
    return state{detail::mixcol(s.c0), detail::mixcol(s.c1),
                 detail::mixcol(s.c2), detail::mixcol(s.c3)};
  }

  static constexpr auto reverse(state const &s) {
    return state{detail::mixcol_inv(s.c0), detail::mixcol_inv(s.c1),
                 detail::mixcol_inv(s.c2), detail::mixcol_inv(s.c3)};
  }
};

// Aes base class
class aes_cipher {
  using round_pipe =
      pipet::pipe<subbyte_filter, shiftrow_filter, mixcol_filter>;
  using last_round_pipe = pipet::helpers::pop_back_t<round_pipe>;

  exp_key<10> const m_k;

public:
  constexpr aes_cipher(serial_key const &k) : m_k{k} {}

  constexpr auto cipher(state s) const {
    for (auto i = 0; i <= 8; ++i) {
      s = round_pipe::process(inject_key(s, m_k.get_at(i)));
    }

    return inject_key(last_round_pipe::process(inject_key(s, m_k.get_at(9))),
                      m_k.get_at(10));
  }

  constexpr auto decipher(state s) const {
    s = last_round_pipe::reverse(inject_key(s, m_k.get_at(10)));

    for (auto i = 9; i >= 1; --i) {
      s = round_pipe::reverse(inject_key(s, m_k.get_at(i)));
    }

    return inject_key(s, m_k.get_at(0));
  }
};

// Just for fun, a variable block size aes entry point (currently just using
// ecb that is known to be weak)
template <size_t N>
constexpr auto aes_ecb_cipher(serial_key const &k,
                              std::array<uint8_t, N> const &plain) {
  constexpr std::size_t sz = N + 16 - N % 16;
  std::array<uint8_t, sz> padded_plain{};

  // fill will be constexpr from c++20
  for (unsigned i = 0; i < sz; ++i) {
    padded_plain[i] = (i < N) ? plain[i] : (i == N ? 0x1 : 0x0);
  }

  std::array<state, sz / 16> ciphered{};
  auto aes_encryptor = aes_cipher{k};
  for (unsigned int i = 0; i < sz; i += 16) {
    ciphered[i / 16] =
        aes_encryptor.cipher(detail::parse_state(&padded_plain[i]));
  }

  return ciphered;
}

template <typename Container, std::size_t N>
auto aes_ecb_decipher(serial_key const &k, std::array<state, N> const &cipher) {
  Container padded_plain{};
  auto aes_encryptor = aes_cipher{k};

  for (unsigned i = 0; i < N; ++i) {
    detail::extend(padded_plain, aes_encryptor.decipher(cipher[i]));
  }

  return detail::remove_pad(padded_plain);
}
} // namespace aes