#include "codegen/llvm_ir_code_generator.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"

auto main() -> int {
  // TODO(andrew): pre-compile stdlib modules

  // auto constexpr source_fname{"examples/hello_world.dao"};
  auto constexpr source_fname{"examples/if_else.dao"};

  auto        tokens{dao::lex(source_fname)};
  dao::parser parser{tokens};

  auto ast{parser.parse()};
  auto analyzer{dao::semantic_analyzer{}};
  auto tables{analyzer.analyze(ast)};
  auto code_generator{dao::llvm_ir_code_generator{source_fname, tables}};

  code_generator.generate(ast);

  // link with clang for now
  // std::system(
  // "clang++-16 -fuse-ld=lld examples/hello_world.dao.o -o hello_world.bin");
  std::system("clang++-16 -fuse-ld=lld examples/if_else.dao.o -o if_else.bin");

  // std::system("/usr/lib/llvm-16/bin/ld.lld examples/hello_world.dao.o -o "
  //             "hello_world -lc -L/usr/lib/x86_64-linux-gnu --entry main");
}
