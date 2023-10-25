#include "fixtures.hpp"
#include "parser/lexer.hpp"

auto directory = Approvals::useApprovalsSubdirectory("golden-files");

auto main() -> int {
  "fibonacci"_test = []() {
    auto lexemes = dao::lex("examples/fibonacci.dao");
    Approvals::verifyAll("fibonacci.dao", lexemes);
  };
}
