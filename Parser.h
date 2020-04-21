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
    Parser(const std::string& filePath, SymbolTables& symbolTables, std::list<SymbolToBeResolved>& symbolsToBeResolved) : m_lexer(filePath), m_symbolTables(symbolTables), m_symbolsToBeResolved(symbolsToBeResolved), m_filePath(filePath), m_returnsValue(false), m_labelCount(0), m_numLocalVariables(0), m_numFieldVariables(0), m_indexOfNumOfFieldsCode(-1) {}
    void parse();

  private:
    Lexer m_lexer;
    SymbolTables& m_symbolTables;
    std::vector<std::string> m_outputCode;
    int m_labelCount;
    int m_numLocalVariables;
    int m_numFieldVariables;
    int m_indexOfNumOfFieldsCode;
    std::list<SymbolToBeResolved>& m_symbolsToBeResolved;
    std::string m_className;
    std::string m_filePath;
    std::string m_scopeReturnType;
    bool m_returnsValue;
    void resolveSymbol(const std::string& name, const Symbol::SymbolKind& symbolKind, const std::vector<std::string>* parameterList);
    bool isClassType(const std::string& symbolType) const;
    bool checkSymbolRedeclaration(const std::string& name, const Symbol::SymbolKind& symbolKind) const;
    void resolveSymbols();
    bool determineIfNeedsToBeResolved(const std::string& symbolName, const Symbol::SymbolKind& symbolKind, std::pair<bool, std::vector<std::string>> parameterList);
    void compareArgumentListToParameterList(const std::vector<std::string>* parameterList, const std::vector<std::string>& expressionListDataTypes) const;

    void printOutputCodeToConsole();
    int getLabelCount() { return m_labelCount++; }

    void jackProgram();
    void classDefinition();
    void memberDefinition();
    void classVariableDefinition();
    void subroutineDefinition();
    void type();
    const std::pair<std::vector<std::string>, std::vector<std::string>> parameterList();
    //returns a boolean indicating whether that body returns a value in all code paths
    bool body();
    void statement();
    void variableDeclarationStatement();
    void letStatement();
    void ifStatement();
    void whileStatement();
    void doStatement();
    void returnStatement();
    //returns a data type of the expression for semantic analyser
    std::string expression();
    void subroutineCall();
    //returns a list of data types for checking parameter lists when calling functions
    const std::vector<std::string> expressionList();
    std::string relationalExpression();
    std::string arithmeticExpression();
    std::string term();
    std::string factor();
    std::string operand();
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