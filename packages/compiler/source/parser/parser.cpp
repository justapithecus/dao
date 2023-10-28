#include "parser.hpp"
#include "ast.hpp"
#include "state_machine.h"
#include "token.hpp"

namespace dao {

  std::unordered_map<char, int> binary_op_precedence = {
    {'<', 10}, // lowest
    {'*', 40}, // highest
  };

  auto parse(std::vector<token> const &tokens) -> ast_node {
    parse_context ctx{tokens};

    for (auto tok{ctx.peek()}; not ctx.is_eof();) {
      return parse_primary_expr(ctx);
    }

    return nullptr;
  }

  auto parse_primary_expr(parse_context &ctx) -> ast_node {
    switch (ctx.peek()->kind) {
    case token_kind_identifier:
      return parse_identifier_expr(ctx);
    case token_kind_numeral:
      return parse_numeral_expr(ctx);
    case token_kind_separator: // TODO(andrew): '(' as separator or special operator
      return parse_parenthetical_expr(ctx);
    case token_kind_operator:
      return parse_binary_expr(ctx);
    default:
      // TODO(andrew): add to ctx.errors
      return nullptr;
    }
  }

  auto parse_identifier_expr(parse_context &ctx) -> ast_node {
    identifier_expr expr{ctx.peek()->repr};
    ctx.eat();
    return std::make_unique<ast>(std::move(expr));
  }

  auto parse_numeral_expr(parse_context &ctx) -> ast_node {
    auto val{std::stoi(ctx.peek()->repr)};
    ctx.eat();
    return std::make_unique<ast>(numeral_expr{val});
  }

  auto parse_parenthetical_expr(parse_context &ctx) -> ast_node {
    // eat '('
    ctx.eat();
    auto node{parse_primary_expr(ctx)};
    // eat ')'
    ctx.eat();
    return node;
  }

  auto parse_binary_expr(parse_context &ctx) -> ast_node {
    auto lhs{parse_primary_expr(ctx)};
    if (!lhs) {
      // TODO(andrew): add to ctx.errors
      return nullptr;
    }

    int constexpr default_op_precedence{0};
    return parse_binary_expr_rhs(ctx, std::move(lhs), default_op_precedence);
  }

  auto parse_binary_expr_rhs(parse_context &ctx, ast_node lhs, int op_precendence)
    -> ast_node {
    for (auto tok{ctx.peek()}; not ctx.is_eof();) {
      auto op{tok->repr[0]};
      auto token_precedence{binary_op_precedence.at(op)};
      if (token_precedence < op_precendence) {
        return lhs;
      }

      // eat operand
      ctx.eat();

      auto rhs{parse_primary_expr(ctx)};
      if (!rhs) {
        // TODO(andrew): add to ctx.errors
        return nullptr;
      }

      // eat operand
      ctx.eat();

      if (not ctx.is_eof()) {
        auto next_op{ctx.peek()->repr[0]};
        auto next_precedence{binary_op_precedence.at(next_op)};
        if (token_precedence < next_precedence) {
          // current right-hand-side becomes the left-hand-side of the inner expression
          rhs = parse_binary_expr_rhs(ctx, std::move(rhs), token_precedence + 1);
          if (!rhs) {
            // TODO(andrew): add to ctx.errors
            return nullptr;
          }
        }
      }

      binary_expr expr{std::move(lhs), std::move(rhs), op};
      lhs = std::make_unique<ast>(std::move(expr));
    }

    return lhs;
  }

} // namespace dao
