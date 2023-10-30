#pragma once

#include <string>

#include "state_machine.h"

namespace dao {

  enum class token_kind : std::uint8_t {
    e_identifier,
    e_numeral,
    e_separator,
    e_operator,
  };

  struct token {
    std::string     repr;
    dao::token_kind kind;
  };

  static_assert(sizeof(dao::token) <= 32);

} // namespace dao
