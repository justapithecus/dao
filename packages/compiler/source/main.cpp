#include <iostream>

#include "codegen/llvm_ir_code_generator.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"

auto main() -> int {
  std::cout << "Hello world!\n";

  auto constexpr source_fname{"examples/hello_world.dao"};

  auto tokens{dao::lex(source_fname)};
  auto ast{dao::parse(tokens)};
  auto code_generator{dao::llvm_ir_code_generator{source_fname}};

  code_generator.generate(ast);
}
