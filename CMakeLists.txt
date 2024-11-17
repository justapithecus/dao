cmake_minimum_required(VERSION 3.30.4)
project(dao
  LANGUAGES C CXX
)

enable_testing()

set(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${DEVBOX_PACKAGES_DIR}")

include(cmake/llvm.cmake)

include_directories(${CMAKE_INCLUDE_PATH}
  SYSTEM
    ${LLVM_INCLUDE_DIRS}
)
link_directories(${CMAKE_LIBRARY_PATH})
link_libraries(${LLVM_LIBS})
#link_libraries(${LLVM_LIBS} stdc++fs)

add_subdirectory(packages/compiler)
add_subdirectory(apps/daoc)
