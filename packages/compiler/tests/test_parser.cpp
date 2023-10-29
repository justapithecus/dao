#include "test_utils.hpp"

#include "parser/parser.hpp"
#include "parser/state_machine.h"

auto directory{Approvals::useApprovalsSubdirectory("golden-files")};

auto main() -> int {

  "identifier_expression_simple"_test = [] {
    auto tokens{load_tokens("identifier_expression_simple.json")};
    auto ast{dao::parse(tokens)};

    json_writer writer{std::move(ast)};
    Approvals::verify(writer);
  };

  "numeral_expression"_test = [] {
    auto tokens{load_tokens("numeral_expression.json")};
    auto ast{dao::parse(tokens)};

    json_writer writer{std::move(ast)};
    Approvals::verify(writer);
  };

  "binary_expression_simple"_test = [] {
    auto               tokens{load_tokens("binary-expressions/simple.json")};
    dao::parse_context ctx{tokens};
    auto               ast{dao::parse_binary_expr(ctx)};

    json_writer writer{std::move(ast)};
    Approvals::verify(writer);
  };

  "binary_expression_operator_precedence_1"_test = [] {
    auto tokens{load_tokens("binary-expressions/operator_precedence_1.json")};
    dao::parse_context ctx{tokens};
    auto               ast{dao::parse_binary_expr(ctx)};

    json_writer writer{std::move(ast)};
    Approvals::verify(writer);
  };
}
