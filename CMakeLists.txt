cmake_minimum_required(VERSION 4.2)
project(kaleidoscope)

set(CMAKE_CXX_STANDARD 23)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Found: ${LLVM_INCLUDE_DIRS}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
llvm_map_components_to_libnames(llvm_libs core)

add_executable(kaleidoscope main.cpp
        lexer.cpp
        ast.h
        parser.cpp
        token.h
        lexer.h
        parser.h
        ast.cpp)

target_link_libraries(kaleidoscope ${llvm_libs})
