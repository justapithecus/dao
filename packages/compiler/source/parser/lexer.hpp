#pragma once

#include <iostream>
#include <string_view>
#include <vector>

#include "token.hpp"

namespace dao {

  struct lexeme {
    std::string  repr;
    std::uint8_t len;
    std::uint8_t line_num;
    std::uint8_t col_num;

    explicit lexeme()
      : len{0}
      , line_num{1}
      , col_num{1} {
    }

    auto update_repr(char const *src_ptr, int end_offset) {
      auto first{src_ptr - len};
      auto last{first + (len + end_offset)};
      repr = std::string{first, last};
    }

    // reset trackers for next lexeme
    auto reset(int offset = 0) {
      col_num += (len + offset);
      len = 0;
    }

    auto as_token(dao::token_kind kind) {
      reset();
      return dao::token{repr, kind};
    }
  };

  static_assert(sizeof(dao::lexeme) <= 32);

  auto lex(std::string_view fpath) -> std::vector<token>;
} // namespace dao
