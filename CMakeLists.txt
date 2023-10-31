cmake_minimum_required(VERSION 3.27.6)
project(dao
  LANGUAGES C CXX
)

enable_testing()

set(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld")
endif()

find_package(LLVM REQUIRED 16 CONFIG)
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
llvm_map_components_to_libnames(LLVM_LIBS core)

# TODO(andrew): figure out why conan cmake presets / toolchain is setting this variable, but
#               not getting picked up automatically...
include_directories(${CMAKE_INCLUDE_PATH} ${LLVM_INCLUDE_DIRS})
link_directories(${CMAKE_LIBRARY_PATH})
link_libraries(${LLVM_LIBS})

add_subdirectory(packages/compiler)
