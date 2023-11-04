#include "parser.hpp"

#include "state_machine.h"

namespace dao {

  auto parse(std::vector<token> const &tokens) -> ast {
    parse_context ctx{tokens};
    return parse(ctx);
  }

  auto parse(parse_context &ctx) -> ast {
    program_ast prog{};

    while (not ctx.is_eof()) {
      switch (ctx.peek()->kind) {
      case token_kind::e_new_line:
        ctx.eat();
        break;
      case token_kind::e_keyword_external:
        prog.nodes.emplace_back(parse_external_linkage(ctx));
        break;
      case token_kind::e_keyword_function:
        // TODO(andrew): implement control flow, keep existing erroneous behavior for now
        if (auto node{parse_function_def(ctx)}) {
          prog.nodes.emplace_back(std::move(node));
          break;
        } else {
          // TODO(andrew): add to ctx.errors
          return prog;
        }

      default:
        if (auto node{parse_expr(ctx)}) {
          prog.entry.emplace_back(std::move(node));
          break;
        } else {
          // TODO(andrew): add to ctx.errors
          return prog;
        }
      }
    }

    return prog;
  }

  auto parse_expr(parse_context &ctx) -> ast_node {
    ctx.skip();

    ast_node node{};

    while (not ctx.is_eof()) {

      switch (ctx.peek()->kind) {
      case token_kind::e_new_line:
        ctx.skip();
        if (node) {
          return node;
        }
        break;
      case token_kind::e_keyword_external:
        // TODO(andrew): errors, external can only be at program/module scope
        return nullptr;
      case token_kind::e_keyword_function:
        node = parse_function_def(ctx);
        break;
      case token_kind::e_keyword_if:
        return parse_if_expr(ctx);
      case token_kind::e_keyword_then:
      case token_kind::e_keyword_else:
        // TODO(andrew): may want to have semicolons...
        return node;
      case token_kind::e_identifier:
        node = parse_identifier_expr(ctx);
        break;
      case token_kind::e_numeral:
        node = parse_numeral_expr(ctx);
        break;
      case token_kind::e_literal:
        node = parse_string_literal(ctx);
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
    case token_kind::e_literal:
      return parse_string_literal(ctx);
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

    if (not ctx.is_eof() and ctx.peek()->repr[0] == '(') {
      return parse_function_call(ctx, std::move(name));
    }

    return std::make_unique<ast>(identifier_expr{std::move(name)});
  }

  template <typename T>
  auto parse_literal(parse_context &ctx) -> ast_node {
    auto val{ctx.peek()->repr};
    ctx.eat();
    return std::make_unique<ast>(T{val});
  }

  auto parse_parenthetical_expr(parse_context &ctx) -> ast_node {
    // TODO(andrew): expected token error-checking
    // eat '('
    ctx.eat();
    auto node{parse_expr(ctx)};
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

    while (not ctx.is_eof() and ctx.peek()->kind == token_kind::e_operator) {
      auto op{ctx.peek()->repr[0]};
      auto token_precedence{
        binary_op_precedence[static_cast<std::uint8_t>(op)]};
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

      if (ctx.peek()->kind == token_kind::e_operator) {
        // eat operand
        auto next_op{ctx.peek()->repr[0]};
        ctx.eat();

        if (not ctx.is_eof()) {
          auto next_precedence{
            binary_op_precedence[static_cast<std::uint8_t>(next_op)]};
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

    // TODO(andrew): maybe some kind of generic parse_sequence with separators
    // TODO(andrew): fix no arg sequence
    while (not ctx.is_eof()) {
      args.emplace_back(parse_function_arg(ctx));

      // eat ',' or ')'
      if (auto sep{ctx.peek()->repr[0]}; sep == ')') {
        ctx.eat();
        return args;
      } else if (sep == ',') {
        ctx.eat();
      } else {
        // TODO(andrew): unexpected end of function arg sequence
        return args;
      }
    }
    return args;
  }

  auto parse_function_proto(parse_context &ctx) -> function_proto {
    // eat 'function'
    ctx.eat();

    // eat function identifier
    auto id{ctx.peek()->repr};
    ctx.eat();

    return function_proto{std::move(id), parse_function_arg_seq(ctx)};
  }

  auto parse_function_def(parse_context &ctx) -> ast_node {
    auto proto{parse_function_proto(ctx)};

    // TODO(andrew): introduce 'external' to distinguish proto-only vs. definition
    auto body{parse_expr(ctx)};
    if (!body) {
      return std::make_unique<ast>(std::move(proto));
    }

    return std::make_unique<ast>(
      function_def{std::move(body), std::move(proto)});
  }

  auto parse_expr_seq(parse_context &ctx) -> std::vector<ast_node> {
    std::vector<ast_node> args{};

    // eat '('
    ctx.eat();

    // TODO(andrew): maybe some kind of generic parse_sequence with separators
    // TODO(andrew): fix no arg sequence
    while (not ctx.is_eof()) {
      args.emplace_back(parse_primary_expr(ctx));

      // eat ',' or ')'
      if (auto sep{ctx.peek()->repr[0]}; sep == ')') {
        ctx.eat();
        return args;
      } else if (sep == ',') {
        ctx.eat();
      } else {
        // TODO(andrew): unexpected end of function call sequence
        return args;
      }
    }

    return args;
  };

  auto parse_function_call(parse_context &ctx, std::string callee) -> ast_node {
    return std::make_unique<ast>(
      function_call{std::move(callee), parse_expr_seq(ctx)});
  }

  ankerl::unordered_dense::map<std::string, linkage_kind> const
    supported_linkages{
      {"C", linkage_kind::e_c_linkage},
    };

  auto parse_external_linkage(parse_context &ctx) -> ast_node {
    // eat 'external'
    ctx.eat();
    // eat '('
    ctx.eat();

    auto it{supported_linkages.find(ctx.peek()->repr)};
    if (it == supported_linkages.end()) {
      // TODO(andrew): ctx.errors, unsupported linkage kind
      return nullptr;
    }
    // eat <external_linkage_kind>, e.g.: "C"
    ctx.eat();
    // eat ')'
    ctx.eat();

    switch (ctx.peek()->kind) {
    case token_kind::e_keyword_function:
      return std::make_unique<ast>(
        external_linkage_ast{it->second, parse_function_proto(ctx)});
    default:
      // TODO(andrew): ctx.errors, external linkage only supports function declarations
      //               eventually also support symbols to other values
      return nullptr;
    }
  }

  auto parse_if_expr(parse_context &ctx) -> ast_node {
    // eat 'if'
    ctx.eat();

    auto condition{parse_expr(ctx)};
    if (!condition) {
      // TODO(andrew): ctx.errors
      return nullptr;
    }

    if (ctx.peek()->kind != token_kind::e_keyword_then) {
      // TODO(andrew): expected 'then'
      return nullptr;
    }

    // eat 'then'
    ctx.eat();
    auto then{parse_expr(ctx)};
    if (!then) {
      // TODO(andrew): ctx.errors
      return nullptr;
    }

    ast_node else_{};
    if (ctx.peek()->kind == token_kind::e_keyword_else) {
      // eat 'else'
      ctx.eat();
      if (else_ = parse_expr(ctx); not else_) {
        // TODO(andrew): ctx.errors
        return nullptr;
      }
    }

    return std::make_unique<ast>(
      if_expr{std::move(condition), std::move(then), std::move(else_)});
  }

} // namespace dao
