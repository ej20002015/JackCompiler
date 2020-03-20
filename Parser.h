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
    Parser(const std::string& filePath, SymbolTables& symbolTables, std::list<SymbolToBeResolved>& symbolsToBeResolved) : m_lexer(filePath), m_symbolTables(symbolTables), m_symbolsToBeResolved(symbolsToBeResolved) {}
    void parse();

  private:
    Lexer m_lexer;
    SymbolTables& m_symbolTables;
    std::list<SymbolToBeResolved>& m_symbolsToBeResolved;
    std::string m_className;
    void resolveSymbol(const std::string& name, const Symbol::SymbolKind& symbolKind);
    bool isClassType(const std::string& symbolType);
    bool checkSymbolRedeclaration(const std::string& name, const Symbol::SymbolKind& symbolKind) const;

    void jackProgram();
    void classDefinition();
    void memberDefinition();
    void classVariableDefinition();
    void subroutineDefinition();
    void type();
    const std::vector<std::string> parameterList();
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

    const std::vector<std::string> m_primitiveDataType
    {
      "int",
      "char",
      "boolean",
      "void"
    };
  };
}