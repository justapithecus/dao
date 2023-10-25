add_executable(daoc)
set_target_properties(daoc
  PROPERTIES
    CXX_STANDARD 20
)

target_sources(daoc
  PRIVATE
    source/main.cpp
    source/parser/lexer.cpp
    source/parser/state_machine.c
)
