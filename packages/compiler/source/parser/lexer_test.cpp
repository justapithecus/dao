#include "../../tests/test_utils.hpp"

#include "parser/lexer.hpp"

auto directory{
  Approvals::useApprovalsSubdirectory("../../tests/golden-files/tokens")};

auto main() -> int {
  // TODO(andrew): output json instead of txt
  "hello_world"_test = [] {
    auto lexemes{dao::lex("examples/hello_world.dao")};
    Approvals::verifyAll("hello_world.dao", lexemes);
  };

  "fibonacci"_test = [] {
    auto lexemes{dao::lex("examples/fibonacci.dao")};
    Approvals::verifyAll("fibonacci.dao", lexemes);
  };
}
