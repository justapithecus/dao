#include "test_utils.hpp"

#include "parser/parser.hpp"
#include "parser/state_machine.h"

auto directory{Approvals::useApprovalsSubdirectory("golden-files")};

auto main() -> int {

  "program"_test = [] {
    auto tokens{load_tokens("program.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };

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

  "function_call_1"_test = [] {
    auto tokens{load_tokens("functions/call_1.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };

  "function_call_2"_test = [] {
    auto tokens{load_tokens("functions/call_2.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };

  // "binary_expression_nested"_test = [] {
  //   auto tokens{load_tokens("binary-expressions/nested.json")};
  //   Approvals::verify(json_writer{dao::parse(tokens)});
  // };

  "control_flow_if_then_expression"_test = [] {
    auto tokens{load_tokens("control-flow/if_then_expression.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };

  "control_flow_if_then_else_expression"_test = [] {
    auto tokens{load_tokens("control-flow/if_then_else_expression.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };

  "control_flow_conditional_puts"_test = [] {
    auto tokens{load_tokens("control-flow/conditional_puts.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };

  "hello_world"_test = [] {
    auto tokens{load_tokens("hello_world.json")};
    Approvals::verify(json_writer{dao::parse(tokens)});
  };
}
