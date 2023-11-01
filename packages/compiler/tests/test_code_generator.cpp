#include "test_utils.hpp"

#include "codegen/llvm_ir_code_generator.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"

auto directory{Approvals::useApprovalsSubdirectory("golden-files")};

auto main() -> int {
  "fibonacci"_test = [] {
    auto constexpr source_fname{"examples/fibonacci.dao"};
    auto tokens{dao::lex(source_fname)};
    auto ast{dao::parse(tokens)};
    auto code_generator{dao::llvm_ir_code_generator{source_fname}};

    std::visit(code_generator, *ast);
    Approvals::verify(code_generator.dumps(),
      ApprovalTests::Options().fileOptions().withFileExtension(".ll"));
  };
}
