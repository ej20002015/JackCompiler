#pragma once

#include "Core.h"
#include "SymbolTable.h"
#include "Lexer.h"

#include <list>

namespace JackCompiler
{
  class Parser
  {
  public:
    Parser(const std::string& filePath, std::list<SymbolTable>& symbolTables, std::list<SymbolToBeResolved>& symbolsToBeResolved) : m_lexer(filePath), m_symbolTables(symbolTables), m_symbolsToBeResolved(symbolsToBeResolved) {}
    void parse();

  private:
    Lexer m_lexer;
    std::list<SymbolTable> m_symbolTables;
    std::list<SymbolToBeResolved> m_symbolsToBeResolved;
    bool checkSymbolExistsInAllSymbolTables(const std::string& name);
    bool checkClassDefined(const std::string& className);
    void resolveSymbol(const std::string& name);

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