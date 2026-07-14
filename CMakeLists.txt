cmake_minimum_required(VERSION 4.2)
project(kaleidoscope)

set(CMAKE_CXX_STANDARD 23)

add_executable(kaleidoscope main.cpp
        lexer.cpp)
