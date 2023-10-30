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
    auto tokens{load_tokens("binary-expressions/simple.json")};
    auto ast{dao::parse(tokens)};

    json_writer writer{std::move(ast)};
    Approvals::verify(writer);
  };

  "binary_expression_operator_precedence_1"_test = [] {
    auto tokens{load_tokens("binary-expressions/operator_precedence_1.json")};
    auto ast{dao::parse(tokens)};

    json_writer writer{std::move(ast)};
    Approvals::verify(writer);
  };

  "parenthetical_expression"_test = [] {
    auto tokens{load_tokens("parenthetical_expression.json")};
    auto ast{dao::parse(tokens)};

    json_writer writer{std::move(ast)};
    Approvals::verify(writer);
  };

  "binary_expression_operator_precedence_2"_test = [] {
    auto tokens{load_tokens("binary-expressions/operator_precedence_2.json")};
    auto ast{dao::parse(tokens)};

    json_writer writer{std::move(ast)};
    Approvals::verify(writer);
  };

  "function_prototype"_test = [] {
    auto tokens{load_tokens("functions/prototype.json")};
    auto ast{dao::parse(tokens)};

    json_writer writer{std::move(ast)};
    Approvals::verify(writer);
  };

  "function_definition"_test = [] {
    auto tokens{load_tokens("functions/definition.json")};
    auto ast{dao::parse(tokens)};

    json_writer writer{std::move(ast)};
    Approvals::verify(writer);
  };

  "function_call"_test = [] {
    auto tokens{load_tokens("functions/call.json")};
    auto ast{dao::parse(tokens)};

    json_writer writer{std::move(ast)};
    Approvals::verify(writer);
  };
}
