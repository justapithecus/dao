#include "codegen/llvm_ir_code_generator.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"

auto compile_module(std::string const &source_fpath, bool emit_main = false) {
  auto        tokens{dao::lex(source_fpath)};
  dao::parser parser{tokens};

  auto ast{parser.parse()};
  auto analyzer{dao::semantic_analyzer{}};
  auto tables{analyzer.analyze(ast)};
  auto code_generator{dao::llvm_ir_code_generator{tables}};

  code_generator.emit(source_fpath, ast, emit_main);
  std::cout << code_generator.dumps(source_fpath) << "\n";
}

auto compile_standard_library() {
  compile_module("packages/std/io.dao");
}

auto link_executable() {
  // link with clang for now
  // std::system(
  // "clang++-16 -fuse-ld=lld examples/hello_world.dao.o -o hello_world.bin");
  std::system("clang++-16 -fuse-ld=lld packages/std/io.dao.o "
              "examples/if_else.dao.o -o if_else.bin");

  // std::system("/usr/lib/llvm-16/bin/ld.lld examples/hello_world.dao.o -o "
  //             "hello_world -lc -L/usr/lib/x86_64-linux-gnu --entry main");
}

auto main() -> int {
  compile_standard_library();

  // auto constexpr source_fpath{"examples/hello_world.dao"};
  auto constexpr source_fpath{"examples/if_else.dao"};
  auto constexpr emit_main{true};

  compile_module(source_fpath, emit_main);
  link_executable();
}
