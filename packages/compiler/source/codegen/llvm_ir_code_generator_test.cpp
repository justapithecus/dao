#include "../../tests/test_utils.hpp"

#include "codegen/llvm_ir_code_generator.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"

auto directory{
  Approvals::useApprovalsSubdirectory("../../tests/golden-files/ir")};

auto main() -> int {
  "triple"_test = [] {
    auto constexpr source_fname{"examples/triple.dao"};
    auto code_generator{dao::llvm_ir_code_generator{source_fname}};

    std::visit(code_generator, dao::parser{dao::lex(source_fname)}.parse());
    Approvals::verify(code_generator.dumps(),
      ApprovalTests::Options().fileOptions().withFileExtension(".ll"));
  };

  "hello_world"_test = [] {
    auto constexpr source_fname{"examples/hello_world.dao"};
    auto code_generator{dao::llvm_ir_code_generator{source_fname}};

    std::visit(code_generator, dao::parser{dao::lex(source_fname)}.parse());
    Approvals::verify(code_generator.dumps(),
      ApprovalTests::Options().fileOptions().withFileExtension(".ll"));
  };

  "if_else"_test = [] {
    auto constexpr source_fname{"examples/if_else.dao"};
    auto code_generator{dao::llvm_ir_code_generator{source_fname}};

    std::visit(code_generator, dao::parser{dao::lex(source_fname)}.parse());
    Approvals::verify(code_generator.dumps(),
      ApprovalTests::Options().fileOptions().withFileExtension(".ll"));
  };
}
