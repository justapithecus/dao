#pragma once

#include <ankerl/unordered_dense.h>
#include <string>

#include "../ast/types.hpp"
#include "state_machine.h"

namespace dao {

  enum class token_kind : std::uint8_t {
    e_end_of_file,
    e_new_line,
    e_identifier,
    e_separator,
    e_operator,
    e_numeral, // TODO(andrew): rename to numeral_literal
    e_string_literal,
    e_keyword_external,
    e_keyword_function,
    e_keyword_if,
    e_keyword_then,
    e_keyword_else,
    e_keyword_alias,
    e_keyword_as,
    e_keyword_ptr,
    e_keyword_mutable,
    e_keyword_readonly,
  };

  enum class operand_kind : std::uint8_t {
    e_single,
    e_multi,
  };

  struct token {
    std::string     repr;
    dao::token_kind kind;

    auto is_type_qualifer() const -> bool {
      return kind == token_kind::e_keyword_mutable or
             kind == token_kind::e_keyword_readonly;
    }

    auto is_type_declarator() const -> bool {
      return kind == token_kind::e_keyword_ptr;
    }

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

  extern ankerl::unordered_dense::map<token_kind, type_qualifier> const
    token_to_type_qualifier;

#ifdef _LIBCPP_VERSION
  // libc++
  static_assert(sizeof(dao::token) <= 32);
#else
  // libstdc++
  static_assert(sizeof(dao::token) <= 40);
#endif

} // namespace dao
