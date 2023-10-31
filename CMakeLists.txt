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

find_package(LLVM REQUIRED 16 CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_definitions(${LLVM_DEFINITIONS})
execute_process(
  COMMAND llvm-config-16 --ldflags
  OUTPUT_VARIABLE LLVM_LD_FLAGS
)
string(STRIP ${LLVM_LD_FLAGS}  LLVM_LD_FLAGS)
message(STATUS "LLVM_LD_FLAGS: ${LLVM_LD_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LLVM_LD_FLAGS}")
execute_process(
  COMMAND llvm-config-16 --libs core
  OUTPUT_VARIABLE LLVM_LIBS
)
string(STRIP ${LLVM_LIBS}  LLVM_LIBS)
message(STATUS "Linking LLVM libs: ${LLVM_LIBS}")

include_directories(${CMAKE_INCLUDE_PATH} ${LLVM_INCLUDE_DIRS})
link_directories(${CMAKE_LIBRARY_PATH})
link_libraries(${LLVM_LIBS})

add_subdirectory(packages/compiler)
