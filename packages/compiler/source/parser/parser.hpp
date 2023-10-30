#pragma once

#include <iostream>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>

#include "ast.hpp"
#include "token.hpp"

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
      return (cursor_ - tokens_.begin()) >= tokens_.size();
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
  auto parse(parse_context &ctx) -> ast_node;

  /// Parses top-level expressions
  ///
  /// <top_level_expr> ::= { <function_def> | <primary_expr> }
  auto parse_top_level_expr(parse_context &ctx) -> ast_node;

  /// Parses any expression
  ///
  /// <expr> ::= { <function_def> | <primary_expr> }
  // TODO(andrew): can be combined with parse_top_level_expr, if a "scope" ast is suitable
  auto parse_expr(parse_context &ctx) -> ast_node;

  /// Parses a primary expression
  ///
  /// <primary_expr> ::= <identifier_expr> | <numeral_expr> | <parenthetical_expr> | <binary_expr>
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
  auto parse_binary_expr(parse_context &ctx, std::uint8_t op_precedence = 0)
    -> ast_node;
  auto parse_binary_expr(parse_context &ctx, ast_node lhs,
    std::uint8_t op_precedence = 0) -> ast_node;

  /// Parses a parenthetical expression, i.e. an expression group with highest evaluation
  /// precedence.
  ///
  /// <parenthetical_expr> ::= '(' <binary_expr> ')'
  auto parse_parenthetical_expr(parse_context &ctx) -> ast_node;

  /// Parses a function argument
  ///
  /// TODO(andrew): add type hints
  /// <function_arg> ::= <identifier>
  auto parse_function_arg(parse_context &ctx) -> function_arg;

  /// Parses a function argument sequence
  ///
  /// <function_arg_seq_multi> ::= <function_arg> | ( <function_arg> ',' <function_arg_seq_multi> )
  /// <function_arg_seq> ::= <empty> | <function_arg_seq_multi>
  auto parse_function_arg_seq(parse_context &ctx) -> std::vector<function_arg>;

  /// Parses a function prototype
  ///
  /// <function_arg> ::= <identifier_seq>
  /// <function_proto> ::= 'function' <identifier_expr> '(' <function_arg_seq> ')'
  auto parse_function_proto(parse_context &ctx) -> function_proto;

  /// Parses a function definition
  ///
  /// <function_def> ::= <function_proto> <primary_expr>
  auto parse_function_def(parse_context &ctx) -> ast_node;

  /// Parses an expression sequence
  ///
  /// <expr_seq_multi> ::= <primary_expr> | ( <primary_expr> ',' <expr_seq_multi> )
  /// <expr_seq> ::= <empty> | <expr_seq_multi>
  auto parse_expr_seq(parse_context &ctx) -> std::vector<ast_node>;

  /// Parses a function call
  ///
  /// <function_call> ::= <identifier_expr> '(' <expr_seq> ')'
  auto parse_function_call(parse_context &ctx, std::string callee) -> ast_node;

} // namespace dao
