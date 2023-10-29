#pragma once

#include <iostream>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>

#include "parser/ast.hpp"
#include "parser/token.hpp"

namespace dao {

  using token_cursor = std::vector<token>::const_iterator;

  class parse_context {
    std::vector<token> const &tokens_;
    token_cursor              cursor_;

    // TODO(andrew): errors

  public:
    explicit parse_context(std::vector<token> const &tokens)
      : tokens_{tokens}
      , cursor_{tokens_.begin()} {
    }

    [[nodiscard]]
    auto peek() const {
      return cursor_;
    }

    [[nodiscard]]
    auto is_eof() const {
      return (cursor_ - tokens_.begin()) > tokens_.size();
    }

    auto rewind() {
      cursor_ = std::prev(cursor_);
      return cursor_;
    }

    auto eat() {
      cursor_ = std::next(cursor_);
      return cursor_;
    }
  };

  /// Parses tokens into an abstract syntax tree
  auto parse(std::vector<token> const &tokens) -> ast_node;

  /// Parses primary expression
  ///
  /// <primary_expr> ::= <identifier_expr> | <numeral_expr> | <binary_expr> | <parenthetical_expr>
  auto parse_primary_expr(parse_context &ctx) -> ast_node;

  /// Parses a simple identifier expression
  ///
  /// <letter> ::= [A-Za-z]
  /// <digit> ::= [0-9]
  /// <identifier_expr> ::= <letter> { <letter> | <digit> | '_' }
  auto parse_identifier_expr(parse_context &ctx) -> ast_node;

  /// Parses a numeral expression
  ///
  /// <digit> ::= [0-9]
  /// <numeral_expr> ::= { <digit> }
  auto parse_numeral_expr(parse_context &ctx) -> ast_node;

  /// Parses a binary expression, i.e. an expression that evaluates two expressions using
  /// a binary operator.
  ///
  ///  TODO(andrew): support more binary operators
  /// <binary_op> ::= { '*' | '+' | '-' | '/' | '>' | '<' }
  /// <binary_expr> ::= <primary_expr> { <binary_op> <primary_expr> }
  auto parse_binary_expr(parse_context &ctx) -> ast_node;

  /// Parses the right-hand-side of a binary expression, which can nest deeply.
  ///
  /// <binary_expr_rhs> ::= { <binary_op> <primary_expr> }
  auto parse_binary_expr_rhs(
    parse_context &ctx, ast_node lhs, int op_precedence = 0) -> ast_node;

  /// Parses a parenthetical expression, i.e. an expression group with highest evaluation
  /// precedence.
  ///
  /// <parenthetical_expr> ::= '(' <binary_expr> ')'
  auto parse_parenthetical_expr(parse_context &ctx) -> ast_node;

} // namespace dao
