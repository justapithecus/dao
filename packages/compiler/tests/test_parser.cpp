#include "test_utils.hpp"

#include "parser/parser.hpp"
#include "parser/state_machine.h"

auto directory = Approvals::useApprovalsSubdirectory("golden-files");

auto main() -> int {

  "simple_identifier_expression"_test = [] {
    auto tokens = load_tokens("simple_identifier_expression.json");
    auto ast    = dao::parse(tokens);

    json_writer writer{std::move(ast)};
    Approvals::verify(writer);
  };

  "numeral_expression"_test = [] {
    auto tokens = load_tokens("numeral_expression.json");
    auto ast    = dao::parse(tokens);

    json_writer writer{std::move(ast)};
    Approvals::verify(writer);
  };
}
