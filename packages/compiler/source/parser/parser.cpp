#include "parser.hpp"
#include "ast.hpp"
#include "state_machine.h"
#include "token.hpp"

namespace dao {

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
    auto node = parse_primary_expr(ctx);
    // eat ')'
    ctx.eat();
    return node;
  }

  auto parse_binary_expr(parse_context &ctx) -> ast_node {
    auto lhs = parse_primary_expr(ctx);
    if (!lhs) {
      // TODO(andrew): add to ctx.errors
      return nullptr;
    }

    auto rhs = parse_primary_expr(ctx);
    char op{};

    binary_expr expr{std::move(lhs), std::move(rhs), op};
    return std::make_unique<ast>(std::move(expr));
  }

} // namespace dao
