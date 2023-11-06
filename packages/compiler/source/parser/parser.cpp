#include "parser.hpp"

#include "state_machine.h"

namespace dao {

  parse_context::parse_context(std::vector<token> const &tokens)
    : tokens_{tokens}
    , cursor_{tokens_.begin()} {
  }

  auto parse_context::is_eof() const {
    return cursor_ != tokens_.end() and
           cursor_->kind == token_kind::e_end_of_file;
  }

  auto parse_context::peek() const {
    return cursor_;
  }

  auto parse_context::seek() {
    cursor_ = std::next(cursor_);
    return cursor_;
  }

  auto parse_context::eat() {
    auto tok{cursor_};
    seek();
    return tok;
  }

  auto parse_context::rewind() {
    cursor_ = std::prev(cursor_);
    return cursor_;
  }

  auto parse_context::skip() {
    while (not is_eof() and cursor_->kind == token_kind::e_new_line) {
      seek();
    }
  }

  parser::parser(std::vector<token> const &tokens)
    : ctx_{tokens} {
  }

  auto parser::parse() -> ast {
    program_ast prog{};

    while (not ctx_.is_eof()) {
      switch (ctx_.peek()->kind) {
      case token_kind::e_new_line:
        ctx_.skip();
        break;
      case token_kind::e_keyword_external:
        prog.nodes.emplace_back(parse_external_linkage());
        break;
      case token_kind::e_keyword_function:
        // TODO(andrew): implement control flow, keep existing erroneous behavior for now
        if (auto node{parse_function_def()}) {
          prog.nodes.emplace_back(std::move(node));
          break;
        } else {
          // TODO(andrew): add to ctx.errors
          return prog;
        }

      default:
        if (auto node{parse_expr()}) {
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

  template <typename T>
  auto parser::parse_literal() -> ast_node {
    auto val{ctx_.eat()->repr};
    return std::make_unique<ast>(T{val});
  }

  template auto parser::parse_literal<numeral_expr>() -> ast_node;
  template auto parser::parse_literal<string_literal>() -> ast_node;

  auto parser::parse_expr() -> ast_node {
    ctx_.skip();

    ast_node node{};

    while (not ctx_.is_eof()) {

      switch (ctx_.peek()->kind) {
      case token_kind::e_new_line:
        // new-line tokens act as terminators
        ctx_.skip();
        [[fallthrough]];
      case token_kind::e_end_of_file:
        if (node) {
          return node;
        }
        break;
      case token_kind::e_keyword_external:
        // TODO(andrew): ctx.errors, currently do not support parsing of external linkages
        //               and functions inside of function definitions
        [[fallthrough]];
      case token_kind::e_keyword_function:
        return nullptr;
      case token_kind::e_keyword_if:
        return parse_if_expr();
      case token_kind::e_keyword_then:
      case token_kind::e_keyword_else:
        // TODO(andrew): may want to have semicolons...
        return node;
      case token_kind::e_identifier:
        node = parse_identifier_expr();
        break;
      case token_kind::e_numeral:
        node = parse_literal<numeral_expr>();
        break;
      case token_kind::e_literal:
        node = parse_literal<string_literal>();
        break;
      case token_kind::e_separator:
        if (ctx_.peek()->repr == "(") {
          node = parse_parenthetical_expr();
        } else if ((ctx_.peek()->repr == ")" or ctx_.peek()->repr == ",") and
                   node) {
          return node;
        } else {
          throw std::runtime_error{"found unexpected separator: " +
                                   ctx_.peek()->repr + " or node was nullptr"};
        }
        break;
      case token_kind::e_operator:
        return parse_binary_expr(std::move(node));
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

  auto parser::parse_primary_expr() -> ast_node {
    switch (ctx_.peek()->kind) {
    case token_kind::e_identifier:
      return parse_identifier_expr();
    case token_kind::e_numeral:
      return parse_literal<numeral_expr>();
    case token_kind::e_literal:
      return parse_literal<string_literal>();
    case token_kind::e_separator:
      if (ctx_.peek()->repr == "(") {
        return parse_parenthetical_expr();
      } else {
        throw std::runtime_error{
          "found unexpected separator: " + ctx_.peek()->repr};
      }
      break;
    case token_kind::e_operator:
      return parse_binary_expr();
    default:
      // TODO(andrew): add to ctx.errors
      return nullptr;
    }
  }

  auto parser::parse_identifier_expr() -> ast_node {
    auto name{ctx_.eat()->repr};

    if (not ctx_.is_eof()) {
      if (ctx_.peek()->as_operand() == '(') {
        return parse_function_call(std::move(name));
      } else if (ctx_.peek()->kind == token_kind::e_operator) {
        auto node{std::make_unique<ast>(identifier_expr{std::move(name)})};
        return parse_binary_expr(std::move(node));
      }
    }

    return std::make_unique<ast>(identifier_expr{std::move(name)});
  }

  auto parser::parse_binary_expr(std::uint8_t op_precedence) -> ast_node {
    auto lhs{parse_primary_expr()};
    if (!lhs) {
      // TODO(andrew): add to ctx.errors
      return nullptr;
    }
    return parse_binary_expr(std::move(lhs), op_precedence);
  }

  auto parser::parse_binary_expr(ast_node lhs, std::uint8_t op_precedence)
    -> ast_node {

    while (not ctx_.is_eof() and ctx_.peek()->kind == token_kind::e_operator) {
      auto op{ctx_.peek()->as_operand()};
      auto token_precedence{
        binary_op_precedence[static_cast<std::uint8_t>(op)]};
      if (token_precedence < op_precedence) {
        return lhs;
      }

      // eat operand
      ctx_.seek();

      auto rhs{parse_primary_expr()};
      if (!rhs) {
        // TODO(andrew): add to ctx.errors
        return nullptr;
      }

      if (ctx_.peek()->kind == token_kind::e_operator) {
        // eat operand
        auto next_op{ctx_.eat()->as_operand()};

        if (not ctx_.is_eof()) {
          auto next_precedence{
            binary_op_precedence[static_cast<std::uint8_t>(next_op)]};
          if (token_precedence <= next_precedence) {
            ctx_.rewind();
            // current right-hand-side becomes the left-hand-side of the inner expression
            rhs = parse_binary_expr(std::move(rhs), token_precedence + 1);
            if (!rhs) {
              // TODO(andrew): add to ctx.errors
              return nullptr;
            }

            if (token_precedence != next_precedence) {
              std::swap(lhs, rhs);
            }
          }
        }
      }

      lhs =
        std::make_unique<ast>(binary_expr{std::move(lhs), std::move(rhs), op});
    }

    return lhs;
  }

  auto parser::parse_parenthetical_expr() -> ast_node {
    // TODO(andrew): expected token error-checking
    // eat '('
    ctx_.seek();
    auto node{parse_expr()};
    // eat ')'
    ctx_.seek();
    return node;
  }

  auto parser::parse_function_arg() -> function_arg {
    auto name{ctx_.eat()->repr};
    return function_arg{std::move(name)};
  }

  auto parser::parse_function_arg_seq() -> std::vector<function_arg> {
    std::vector<function_arg> args{};

    // eat '('
    ctx_.seek();

    // no-args
    if (auto sep{ctx_.peek()->as_operand()}; sep == ')') {
      ctx_.seek();
      return args;
    }

    // TODO(andrew): maybe some kind of generic parse_sequence with separators
    while (not ctx_.is_eof()) {
      args.emplace_back(parse_function_arg());

      // eat ',' or ')'
      if (auto sep{ctx_.peek()->as_operand()}; sep == ')') {
        ctx_.seek();
        return args;
      } else if (sep == ',') {
        ctx_.seek();
      } else {
        // TODO(andrew): unexpected end of function arg sequence
        return args;
      }
    }
    return args;
  }

  auto parser::parse_function_proto() -> function_proto {
    // eat 'function'
    ctx_.seek();

    // eat function identifier
    auto id{ctx_.eat()->repr};
    return function_proto{std::move(id), parse_function_arg_seq()};
  }

  auto parser::parse_function_def() -> ast_node {
    auto proto{parse_function_proto()};

    ctx_.skip();

    // currently do not support defining functions or declaring prototypes
    // inside of function bodies
    if (ctx_.peek()->kind == token_kind::e_keyword_function or
        ctx_.peek()->kind == token_kind::e_keyword_external) {
      return std::make_unique<ast>(std::move(proto));
    }

    auto body{parse_expr()};
    if (not body) {
      return std::make_unique<ast>(std::move(proto));
    }

    return std::make_unique<ast>(
      function_def{std::move(body), std::move(proto)});
  }

  auto parser::parse_expr_seq() -> std::vector<ast_node> {
    std::vector<ast_node> args{};

    // eat '('
    ctx_.seek();

    // no-args
    if (auto sep{ctx_.peek()->as_operand()}; sep == ')') {
      ctx_.seek();
      return args;
    }

    // TODO(andrew): maybe some kind of generic parse_sequence with separators
    while (not ctx_.is_eof()) {
      args.emplace_back(parse_primary_expr());

      // eat ',' or ')'
      if (auto sep{ctx_.peek()->as_operand()}; sep == ')') {
        ctx_.seek();
        return args;
      } else if (sep == ',') {
        ctx_.seek();
      } else {
        // TODO(andrew): unexpected end of function call sequence
        return args;
      }
    }

    return args;
  };

  auto parser::parse_function_call(std::string callee) -> ast_node {
    return std::make_unique<ast>(
      function_call{std::move(callee), parse_expr_seq()});
  }

  ankerl::unordered_dense::map<std::string, linkage_kind> const
    supported_linkages{
      {"C", linkage_kind::e_c_linkage},
    };

  auto parser::parse_external_linkage() -> ast_node {
    // eat 'external'
    ctx_.seek();
    // eat '('
    ctx_.seek();

    // eat <external_linkage_kind>, e.g.: "C"
    auto it{supported_linkages.find(ctx_.eat()->repr)};
    if (it == supported_linkages.end()) {
      // TODO(andrew): ctx.errors, unsupported linkage kind
      return nullptr;
    }
    // eat ')'
    ctx_.seek();

    switch (ctx_.peek()->kind) {
    case token_kind::e_keyword_function:
      return std::make_unique<ast>(
        external_linkage_ast{it->second, parse_function_proto()});
    default:
      // TODO(andrew): ctx.errors, external linkage only supports function declarations
      //               eventually also support symbols to other values
      return nullptr;
    }
  }

  auto parser::parse_if_expr() -> ast_node {
    // eat 'if'
    ctx_.seek();

    auto condition{parse_expr()};
    if (!condition) {
      // TODO(andrew): ctx.errors
      return nullptr;
    }

    if (ctx_.peek()->kind != token_kind::e_keyword_then) {
      // TODO(andrew): expected 'then'
      return nullptr;
    }

    // eat 'then'
    ctx_.seek();
    auto then{parse_expr()};
    if (!then) {
      // TODO(andrew): ctx.errors
      return nullptr;
    }

    ast_node else_{};
    if (ctx_.peek()->kind == token_kind::e_keyword_else) {
      // eat 'else'
      ctx_.seek();
      if (else_ = parse_expr(); not else_) {
        // TODO(andrew): ctx.errors
        return nullptr;
      }
    }

    return std::make_unique<ast>(
      if_expr{std::move(condition), std::move(then), std::move(else_)});
  }

} // namespace dao
