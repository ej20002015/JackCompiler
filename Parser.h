#pragma once

#include "Core.h"
#include "Lexer.h"

namespace JackCompiler
{
  class Parser
  {
  public:
    Parser(const std::string& filePath) : m_lexer(filePath) {}
    void parse();

  private:
    Lexer m_lexer;
    void jackProgram();
    void classDefinition();
    void memberDefinition();
    void classVariableDefinition();
    void subroutineDefinition();
    void type();
    void parameterList();
    void subroutineBody();
    void statement();
    void variableDeclarationStatement();
    void letStatement();
    void ifStatement();
    void whileStatement();
    void doStatement();
    void returnStatement();
  };
}