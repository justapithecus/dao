# TODO(andrew): this could be written better
function (add_compiler_test TARGET ENTRY)
  set(TEST_EXE test_${TARGET})

  add_test(
    NAME ${TEST_EXE}
    COMMAND ${TEST_EXE}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
  add_executable(${TEST_EXE} ${ENTRY})
  target_include_directories(${TEST_EXE}
    PRIVATE
      ${CMAKE_SOURCE_DIR}/include/shared
      ${CMAKE_CURRENT_SOURCE_DIR}/../source
  )
  target_compile_definitions(${TEST_EXE}
    PRIVATE
      CTEST=1
  )
  # TODO(andrew): clean this up
  target_sources(${TEST_EXE}
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/../source/codegen/llvm_ir_code_generator.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/../source/parser/lexer.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/../source/parser/parser.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/../source/parser/state_machine.c
      ${CMAKE_CURRENT_SOURCE_DIR}/../source/serialization/ast_pretty_printer.cpp
  )

endfunction()

add_compiler_test(lexer test_lexer.cpp)
add_compiler_test(parser test_parser.cpp)
add_compiler_test(code_generator test_code_generator.cpp)
