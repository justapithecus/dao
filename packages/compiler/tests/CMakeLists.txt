# TODO(andrew): this could be written better
function (add_compiler_test TARGET ENTRY)
  set(TEST_EXE test_${TARGET})

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
      ${CMAKE_CURRENT_SOURCE_DIR}/../source/parser/lexer.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/../source/parser/state_machine.c
  )

endfunction()

add_compiler_test(lexer test_lexer.cpp)
