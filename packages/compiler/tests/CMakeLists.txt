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
      ${CMAKE_SOURCE_DIR}/packages/compiler/source
  )
  target_compile_definitions(${TEST_EXE}
    PRIVATE
      CTEST=1
  )
  target_sources(${TEST_EXE}
    PRIVATE
      ${CMAKE_SOURCE_DIR}/packages/compiler/source/serialization/ast_pretty_printer.cpp
  )
  target_link_libraries(${TEST_EXE}
    PRIVATE
      libdao_compiler
  )
endfunction()

add_compiler_test(lexer ${CMAKE_SOURCE_DIR}/packages/compiler/source/parser/lexer_test.cpp)
add_compiler_test(parser ${CMAKE_SOURCE_DIR}/packages/compiler/source/parser/parser_test.cpp)
add_compiler_test(llvm_ir_code_generator ${CMAKE_SOURCE_DIR}/packages/compiler/source/codegen/llvm_ir_code_generator_test.cpp)
