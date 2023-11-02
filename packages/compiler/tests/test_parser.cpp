#include "test_utils.hpp"

#include "parser/parser.hpp"
#include "parser/state_machine.h"

auto directory{Approvals::useApprovalsSubdirectory("golden-files")};

auto main() -> int {

  "identifier_expression_simple"_test = [] {
    auto tokens{load_tokens("identifier_expression_simple.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };

  "numeral_expression"_test = [] {
    auto tokens{load_tokens("numeral_expression.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };

  "binary_expression_simple"_test = [] {
    auto tokens{load_tokens("binary-expressions/simple.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };

  "binary_expression_operator_precedence_1"_test = [] {
    auto tokens{load_tokens("binary-expressions/operator_precedence_1.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };

  "parenthetical_expression"_test = [] {
    auto tokens{load_tokens("parenthetical_expression.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };

  "binary_expression_operator_precedence_2"_test = [] {
    auto tokens{load_tokens("binary-expressions/operator_precedence_2.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };

  "function_prototype"_test = [] {
    auto tokens{load_tokens("functions/prototype.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };

  "function_definition"_test = [] {
    auto tokens{load_tokens("functions/definition.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };

  "function_call"_test = [] {
    auto tokens{load_tokens("functions/call.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };

  "program"_test = [] {
    auto tokens{load_tokens("program.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };

  "hello_world"_test = [] {
    auto tokens{load_tokens("hello_world.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };
}
