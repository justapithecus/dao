function (add_compiler_test TARGET ENTRY)
  set(TEST_EXE test_${TARGET})

  add_test(
    NAME ${TEST_EXE}
    COMMAND ${TEST_EXE}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )
  add_executable(${TEST_EXE}
    ${ENTRY}
    source/serialization/ast_pretty_printer.cpp
  )
  target_include_directories(${TEST_EXE}
    PRIVATE
      ${CMAKE_SOURCE_DIR}/include/shared
      ${CMAKE_SOURCE_DIR}/packages/compiler/source
  )
  target_compile_definitions(${TEST_EXE}
    PRIVATE
      CTEST=1
  )
  target_link_libraries(${TEST_EXE}
    PRIVATE
      libdao_compiler
  )
endfunction()
