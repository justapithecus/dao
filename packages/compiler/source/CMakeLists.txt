add_executable(daoc)
set_target_properties(daoc
  PROPERTIES
    CXX_STANDARD 20
)

target_sources(daoc
  PRIVATE
    main.cpp
    parser/lexer.cpp
    parser/state_machine.c
    parser/token.cpp
)
