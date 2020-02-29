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
    void body();
    void statement();
    void variableDeclarationStatement();
    void letStatement();
    void ifStatement();
    void whileStatement();
    void doStatement();
    void returnStatement();
    void expression();
    void subroutineCall();
    void expressionList();
    void relationalExpression();
    void arithmeticExpression();
    void term();
    void factor();
    void operand();
    bool isExpression(const Token& token);

    const std::vector<std::string> m_possibleStartTokensOfExpression
    {
      "-",
      "~",
      "(",
      "true",
      "false",
      "null",
      "this"
    };
  };
}