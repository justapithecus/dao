#include "parser.hpp"
#include "ast.hpp"
#include "state_machine.h"
#include "token.hpp"

namespace dao {

  auto parse(std::vector<dao::token> const &tokens) -> std::unique_ptr<ast_node> {
    parse_context ctx{tokens};

    for (auto tok{ctx.peek()}; not ctx.is_eof();) {

      switch (tok->kind) {
      case token_kind_identifier:
        return parse_identifier_expr(ctx);
      case token_kind_numeral:
        return parse_numeral_expr(ctx);
      }
    }

    return nullptr;
  }

  auto parse_identifier_expr(parse_context &ctx) -> std::unique_ptr<dao::ast_node> {
    identifier_expr expr{ctx.peek()->repr};
    ctx.eat();
    return std::make_unique<dao::ast_node>(std::move(expr));
  }

  auto parse_numeral_expr(parse_context &ctx) -> std::unique_ptr<dao::ast_node> {
    auto val{std::stoi(ctx.peek()->repr)};
    ctx.eat();
    return std::make_unique<dao::ast_node>(numeral_expr{val});
  }

} // namespace dao
