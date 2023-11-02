add_executable(daoc)
set_target_properties(daoc
  PROPERTIES
    CXX_STANDARD 20
)

target_sources(daoc
  PRIVATE
    main.cpp
)

target_include_directories(daoc
  PRIVATE
    ${CMAKE_SOURCE_DIR}/packages/compiler/source
)

target_link_libraries(daoc
  PRIVATE
    libdao_compiler
)
