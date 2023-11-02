#include "test_utils.hpp"

#include "fixtures.hpp"
#include "parser/lexer.hpp"

auto directory = Approvals::useApprovalsSubdirectory("golden-files");

auto main() -> int {
  "hello_world"_test = [] {
    auto lexemes{dao::lex("examples/hello_world.dao")};
    Approvals::verifyAll("hello_world.dao", lexemes);
  };

  "fibonacci"_test = [] {
    auto lexemes{dao::lex("examples/fibonacci.dao")};
    Approvals::verifyAll("fibonacci.dao", lexemes);
  };
}
