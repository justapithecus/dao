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
set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld")

# TODO(andrew): figure out why conan cmake presets / toolchain is setting this variable, but
#               not getting picked up automatically...
include_directories(${CMAKE_INCLUDE_PATH})
link_directories(${CMAKE_LIBRARY_PATH})

add_subdirectory(packages/compiler)
