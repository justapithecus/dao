#pragma once

#include <ankerl/unordered_dense.h>
#include <string>

#include "state_machine.h"

namespace dao {

  enum class token_kind : std::uint8_t {
    e_end_of_file,
    e_new_line,
    e_identifier,
    e_separator,
    e_operator,
    e_numeral,
    e_literal, // TODO(andrew): distinguish string_literal vs. numeral_literal eventually
    e_keyword_external,
    e_keyword_function,
    e_keyword_if,
    e_keyword_then,
    e_keyword_else,
    e_keyword_alias,
    e_keyword_as,
  };

  enum class operand_kind : std::uint8_t {
    e_single,
    e_multi,
  };

  struct token {
    std::string     repr;
    dao::token_kind kind;

    template <operand_kind Op = operand_kind::e_single>
    auto as_operand() const noexcept {
      return repr[0];
    }

    template <>
    auto as_operand<operand_kind::e_multi>() const noexcept {
      return repr;
    }
  };

  extern ankerl::unordered_dense::map<std::string, dao::token_kind> const
    keywords;

#ifdef _LIBCPP_VERSION
  // libc++
  static_assert(sizeof(dao::token) <= 32);
#else
  // libstdc++
  static_assert(sizeof(dao::token) <= 40);
#endif

} // namespace dao
