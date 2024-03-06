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
      std::string{test_path} + "golden-files/llvm/" + std::string{name} +
      ".{ApprovedOrReceived}.ll")};
    auto constexpr emit_main{true};

    test(name) = [&] {
      // TODO(andrew): also need to load approved tables from file, so as to not be redundant
      auto ast{dao::parser{dao::lex(path.generic_string())}.parse()};
      auto analyzer{dao::semantic_analyzer{}};
      auto tables{analyzer.analyze(ast)};
      auto code_generator{dao::llvm_ir_code_generator{tables}};
      auto mod_id{code_generator.generate(filename, ast, emit_main)};

      auto opts{ApprovalTests::Options()
                  .fileOptions()
                  .withFileExtension(".ll")
                  .withNamer(namer)};
      Approvals::verify(code_generator.dumps(mod_id), opts);
    };
  }
}
