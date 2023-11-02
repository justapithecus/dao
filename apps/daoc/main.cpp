#include "codegen/llvm_ir_code_generator.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"

auto main() -> int {
  // TODO(andrew): pre-compile modules

  auto constexpr source_fname{"examples/hello_world.dao"};

  auto tokens{dao::lex(source_fname)};
  auto ast{dao::parse(tokens)};
  auto code_generator{dao::llvm_ir_code_generator{source_fname}};

  code_generator.generate(ast);

  // link with clang for now
  std::system("clang++ examples/hello_world.dao.o -o hello_world");

  // std::system("/usr/lib/llvm-16/bin/ld.lld examples/hello_world.dao.o -o "
  //             "hello_world -lc -L/usr/lib/x86_64-linux-gnu --entry main");
}
