#include "../../tests/test_utils.hpp"

#include "parser/parser.hpp"
#include "parser/state_machine.h"

auto directory{
  Approvals::useApprovalsSubdirectory("../../tests/golden-files/ast")};

auto main() -> int {

  for (auto const &entry : load_test_cases_tokens()) {
    auto path{entry.path()};
    auto name{path.stem().generic_string()};
    if (entry.is_regular_file() and name.find("tokens") != std::string::npos) {
      auto filename{path.filename().generic_string()};
      auto base_name{path.stem().stem().stem().generic_string()};
      auto namer{TemplatedCustomNamer::create(
        std::string{test_path} + "golden-files/" + std::string{base_name} +
        ".ast.{ApprovedOrReceived}.{FileExtension}")};

      test(name) = [&] {
        auto tokens{load_tokens(path.generic_string())};
        json contents{
          {"_filename", filename},
          {"ast", dao::parser{tokens}.parse()},
        };

        Approvals::verify(json_writer{contents}, Options().withNamer(namer));
      };
    }
  }

  // "program"_test = [] {
  //   auto tokens{load_tokens("program.json")};
  //   Approvals::verify(json_writer{dao::parser{tokens}.parse()});
  // };

  // "identifier_expression_simple"_test = [] {
  //   auto tokens{load_tokens("identifier_expression_simple.json")};
  //   Approvals::verify(json_writer{dao::parser{tokens}.parse()});
  // };

  // "numeral_expression"_test = [] {
  //   auto tokens{load_tokens("numeral_expression.json")};
  //   Approvals::verify(json_writer{dao::parser{tokens}.parse()});
  // };

  // "binary_expression_simple"_test = [] {
  //   auto tokens{load_tokens("binary-expressions/simple.json")};
  //   Approvals::verify(json_writer{dao::parser{tokens}.parse()});
  // };

  // "binary_expression_operator_precedence_1"_test = [] {
  //   auto tokens{load_tokens("binary-expressions/operator_precedence_1.json")};
  //   Approvals::verify(json_writer{dao::parser{tokens}.parse()});
  // };

  // "parenthetical_expression"_test = [] {
  //   auto tokens{load_tokens("parenthetical_expression.json")};
  //   Approvals::verify(json_writer{dao::parser{tokens}.parse()});
  // };

  // "binary_expression_operator_precedence_2"_test = [] {
  //   auto tokens{load_tokens("binary-expressions/operator_precedence_2.json")};
  //   Approvals::verify(json_writer{dao::parser{tokens}.parse()});
  // };

  // "function_prototype"_test = [] {
  //   auto tokens{load_tokens("functions/prototype.json")};
  //   Approvals::verify(json_writer{dao::parser{tokens}.parse()});
  // };

  // "function_definition"_test = [] {
  //   auto tokens{load_tokens("functions/definition.json")};
  //   Approvals::verify(json_writer{dao::parser{tokens}.parse()});
  // };

  // "function_call_1"_test = [] {
  //   auto tokens{load_tokens("functions/call_1.json")};
  //   Approvals::verify(json_writer{dao::parser{tokens}.parse()});
  // };

  // "function_call_2"_test = [] {
  //   auto tokens{load_tokens("functions/call_2.json")};
  //   Approvals::verify(json_writer{dao::parser{tokens}.parse()});
  // };

  // "binary_expression_nested"_test = [] {
  //   auto tokens{load_tokens("binary-expressions/nested.json")};
  //   Approvals::verify(json_writer{dao::parser{tokens}.parse()});
  // };

  // "control_flow_if_then_expression"_test = [] {
  //   auto tokens{load_tokens("control-flow/if_then_expression.json")};
  //   Approvals::verify(json_writer{dao::parser{tokens}.parse()});
  // };

  // "control_flow_if_then_else_expression"_test = [] {
  //   auto tokens{load_tokens("control-flow/if_then_else_expression.json")};
  //   Approvals::verify(json_writer{dao::parser{tokens}.parse()});
  // };

  // "control_flow_conditional_puts"_test = [] {
  //   auto tokens{load_tokens("control-flow/conditional_puts.json")};
  //   Approvals::verify(json_writer{dao::parser{tokens}.parse()});
  // };

  // "hello_world"_test = [] {
  //   auto tokens{load_tokens("hello_world.json")};
  //   Approvals::verify(json_writer{dao::parser{tokens}.parse()});
  // };
}
