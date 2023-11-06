add_library(libdao_compiler
  STATIC
    source/analysis/semantic_analyzer.cpp
    source/codegen/llvm_ir_code_generator.cpp
    source/parser/lexer.cpp
    source/parser/parser.cpp
    source/parser/state_machine.c
    source/parser/token.cpp
)

set_target_properties(libdao_compiler
  PROPERTIES
    CXX_STANDARD 20
)

include(test.cmake)
add_compiler_test(lexer source/parser/lexer_test.cpp)
add_compiler_test(parser source/parser/parser_test.cpp)
add_compiler_test(llvm_ir_code_generator source/codegen/llvm_ir_code_generator_test.cpp)
