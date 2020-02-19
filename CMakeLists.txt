project("JackCompiler")

set(CMAKE_BUILD_TYPE Debug)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-lm -std=c++17")

add_executable(JackCompiler
    main.cpp
    Compiler.cpp
    Lexer.cpp
)