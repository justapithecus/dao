#pragma once

#include <string>

#include "state_machine.h"

namespace dao {

  enum class token_kind : std::uint8_t {
    e_identifier,
    e_numeral,
    e_separator,
    e_operator,
    e_literal, // TODO(andrew): distinguish string_literal vs. numeral_literal eventually
    e_keyword,
  };

  struct token {
    std::string     repr;
    dao::token_kind kind;
  };

#ifdef _LIBCPP_VERSION
  // libc++
  static_assert(sizeof(dao::token) <= 32);
#else
  // libstdc++
  static_assert(sizeof(dao::token) <= 40);
#endif

} // namespace dao
