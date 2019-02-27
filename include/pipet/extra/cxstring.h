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

#include <cstdint>
#include <string>
#include <utility>

namespace pipet::extra {
template <std::size_t N> class cxstring {
  using data_type = char const (&)[N];
  char const m_data[N];

public:
  constexpr cxstring(data_type s)
      : cxstring(s, std::make_index_sequence<N>{}) {}

  constexpr auto size() const { return N; }

  constexpr auto operator[](std::size_t idx) const { return m_data[idx]; }

  operator std::string() const { return std::string{m_data, N - 1}; }

private:
  template <typename S, std::size_t... Is>
  constexpr cxstring(S s, std::index_sequence<Is...>) : m_data{s[Is]...} {}
};

// factory method for compiler version without deduction guide
template <std::size_t N> constexpr auto make_cxstring(char const (&s)[N]) {
  return cxstring<N>(s);
}
} // namespace pipet::extra