#include "../../tests/test_utils.hpp"

#include "parser/lexer.hpp"

auto directory{
  Approvals::useApprovalsSubdirectory("../../tests/golden-files/tokens")};

auto main() -> int {

  for (auto const &file : load_test_examples()) {
    auto path{file.path()};
    auto name{path.stem().c_str()};
    auto filename{path.filename().c_str()};

    test(name) = [&] {
      auto namer{TemplatedCustomNamer::create(
        "packages/compiler/tests/golden-files/tokens/" + std::string{name} +
        ".{ApprovedOrReceived}.{FileExtension}")};

      Approvals::verifyAll(
        filename, dao::lex(path.c_str()), Options().withNamer(namer));
    };
  }

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
