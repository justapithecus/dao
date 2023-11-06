#include "../../tests/test_utils.hpp"

#include "../parser/lexer.hpp"
#include "../parser/parser.hpp"
#include "llvm_ir_code_generator.hpp"

auto main() -> int {
  // use approved lexer outputs as inputs to parser
  for (auto const &file : load_test_cases("programs")) {
    auto path{file.path()};
    auto name{path.stem().generic_string()};
    auto filename{path.filename().generic_string()};
    auto namer{TemplatedCustomNamer::create(
      std::string{test_path} + "golden-files/ir/" + std::string{name} +
      ".{ApprovedOrReceived}.ll")};

    test(name) = [&] {
      auto code_generator{dao::llvm_ir_code_generator{filename}};
      auto ast{dao::parser{dao::lex(path.generic_string())}.parse()};
      std::visit(code_generator, std::move(ast));

      auto opts{ApprovalTests::Options()
                  .fileOptions()
                  .withFileExtension(".ll")
                  .withNamer(namer)};
      Approvals::verify(code_generator.dumps(), opts);
    };
  }
}
