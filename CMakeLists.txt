project("JackCompiler")

set(CMAKE_BUILD_TYPE Release)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-lm -std=c++11")

add_executable(JackCompiler
    main.cpp
    Compiler.cpp
    Lexer.cpp
    Core.cpp
    Parser.cpp
    SymbolTable.cpp
)

set_property(TARGET JackCompiler PROPERTY CXX_STANDARD 11)