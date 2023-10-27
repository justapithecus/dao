#pragma once

#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>

#include "parser/ast.hpp"
#include "parser/token.hpp"

namespace dao {

  using token_cursor = std::vector<dao::token>::const_iterator;

  class parse_context {
    std::vector<dao::token> const &tokens_;
    token_cursor                   cursor_;

  public:
    explicit parse_context(std::vector<dao::token> const &tokens)
      : tokens_{tokens}
      , cursor_{tokens.begin()} {
    }

    [[nodiscard]]
    auto peek() const {
      return cursor_;
    }

    [[nodiscard]]
    auto is_eof() const {
      return cursor_ == tokens_.end();
    }

    auto eat() {
      cursor_ = std::next(cursor_);
      return cursor_;
    }
  };

  auto parse(std::vector<dao::token> const &tokens) -> std::unique_ptr<dao::ast_node>;

  /// Parses a simple identifier expression
  ///
  /// <letter> ::= [A-Za-z]
  /// <number> ::= [0-9]
  /// <identifier_expr> ::= <letter> ( <letter> | <number> | [_] )*
  auto parse_identifier_expr(parse_context &ctx) -> std::unique_ptr<dao::ast_node>;

  /// Parses a numeral expression
  ///
  /// <number> ::= [0-9]
  /// <numeral_expr> ::= <number>*
  auto parse_numeral_expr(parse_context &ctx) -> std::unique_ptr<dao::ast_node>;

} // namespace dao
