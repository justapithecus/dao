add_library(libdao_compiler
  STATIC
    codegen/llvm_ir_code_generator.cpp
    parser/lexer.cpp
    parser/parser.cpp
    parser/state_machine.c
    parser/token.cpp
)

set_target_properties(libdao_compiler
  PROPERTIES
    CXX_STANDARD 20
)
