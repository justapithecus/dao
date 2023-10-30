#include "parser.hpp"

#include "state_machine.h"

namespace dao {

  auto parse(std::vector<token> const &tokens) -> ast_node {
    parse_context ctx{tokens};
    return parse(ctx);
  }

  // TODO(andrew): this top-level parse should carry a "Scope" ast_node
  auto parse(parse_context &ctx) -> ast_node {
    ast_node node{};

    for (auto tok{ctx.peek()}; not ctx.is_eof();) {

      switch (ctx.peek()->kind) {
      case token_kind::e_keyword:
        node = parse_function_proto(ctx);
        break;
      case token_kind::e_identifier:
        node = parse_identifier_expr(ctx);
        break;
      case token_kind::e_numeral:
        node = parse_numeral_expr(ctx);
        break;
      case token_kind::e_separator:
        node = parse_parenthetical_expr(ctx);
        break;
      case token_kind::e_operator:
        return parse_binary_expr(ctx, std::move(node));
      default:
        // TODO(andrew): add to ctx.errors, encountered unknown
        break;
      }

      if (!node) {
        // TODO(andrew): add to ctx.errors
        return nullptr;
      }
    }

    return node;
  }

  auto parse_primary_expr(parse_context &ctx) -> ast_node {
    switch (ctx.peek()->kind) {
    case token_kind::e_identifier:
      return parse_identifier_expr(ctx);
    case token_kind::e_numeral:
      return parse_numeral_expr(ctx);
    case token_kind::e_separator:
      return parse_parenthetical_expr(ctx);
    case token_kind::e_operator:
      return parse_binary_expr(ctx);
    default:
      // TODO(andrew): add to ctx.errors
      return nullptr;
    }
  }

  auto parse_identifier_expr(parse_context &ctx) -> ast_node {
    auto name{ctx.peek()->repr};
    ctx.eat();
    return std::make_unique<ast>(identifier_expr{std::move(name)});
  }

  auto parse_numeral_expr(parse_context &ctx) -> ast_node {
    auto val{std::stoi(ctx.peek()->repr)};
    ctx.eat();
    return std::make_unique<ast>(numeral_expr{val});
  }

  auto parse_parenthetical_expr(parse_context &ctx) -> ast_node {
    // TODO(andrew): expected token error-checking
    // eat '('
    ctx.eat();
    auto node{parse(ctx)};
    // eat ')'
    ctx.eat();
    return node;
  }

  auto parse_binary_expr(parse_context &ctx, std::uint8_t op_precedence)
    -> ast_node {
    auto lhs{parse_primary_expr(ctx)};
    if (!lhs) {
      // TODO(andrew): add to ctx.errors
      return nullptr;
    }

    return parse_binary_expr(ctx, std::move(lhs), op_precedence);
  }

  // TODO(andrew): simplify this such that binary_expr can be parsed as a primary_expr
  auto parse_binary_expr(
    parse_context &ctx, ast_node lhs, std::uint8_t op_precedence) -> ast_node {

    for (auto tok{ctx.peek()};
         not ctx.is_eof() and tok->kind == token_kind::e_operator;) {

      auto op{tok->repr[0]};
      auto token_precedence{binary_op_precedence[op]};
      if (token_precedence < op_precedence) {
        return lhs;
      }

      // eat operand
      ctx.eat();

      auto rhs{parse_primary_expr(ctx)};
      if (!rhs) {
        // TODO(andrew): add to ctx.errors
        return nullptr;
      }

      if (tok = {ctx.peek()}; tok->kind == token_kind::e_operator) {
        // eat operand
        auto next_op{tok->repr[0]};
        ctx.eat();

        if (not ctx.is_eof()) {
          auto next_precedence{binary_op_precedence[next_op]};
          if (token_precedence < next_precedence) {
            // current right-hand-side becomes the left-hand-side of the inner expression
            ctx.rewind();
            rhs = parse_binary_expr(ctx, std::move(rhs), token_precedence + 1);
            if (!rhs) {
              // TODO(andrew): add to ctx.errors
              return nullptr;
            }

            std::swap(lhs, rhs);
          }
        }
      }

      lhs =
        std::make_unique<ast>(binary_expr{std::move(lhs), std::move(rhs), op});
    }

    return lhs;
  }

  auto parse_function_arg(parse_context &ctx) -> function_arg {
    auto name{ctx.peek()->repr};
    ctx.eat();
    return function_arg{std::move(name)};
  }

  auto parse_function_arg_seq(parse_context &ctx) -> std::vector<function_arg> {
    std::vector<function_arg> args{};

    // eat '('
    ctx.eat();

    for (auto tok{ctx.peek()}; not ctx.is_eof();) {
      args.emplace_back(parse_function_arg(ctx));

      // eat ',' or ')'
      switch (auto sep{ctx.peek()->repr[0]}; sep) {
      case ')':
        ctx.eat();
        return args;
      case ',':
        ctx.eat();
        break;
      default:
        // TODO(andrew): unexpected end of function arg sequence
        return args;
      }
    }
    return args;
  }

  auto parse_function_proto(parse_context &ctx) -> ast_node {
    // eat 'function'
    ctx.eat();

    // eat function identifier
    auto id{ctx.peek()->repr};
    ctx.eat();

    return std::make_unique<ast>(
      function_proto{std::move(id), parse_function_arg_seq(ctx)});
  }

} // namespace dao
