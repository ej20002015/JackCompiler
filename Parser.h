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
    /**
    * compile the file by performing lexical analysis and syntactical analysis, whilst checking the semantics and generating the target vm code
    */
    void parse();
    const std::vector<std::string>& getOutputCode() const { return m_outputCode; }

  private:
    //Lexer object to tokenise the input file
    Lexer m_lexer;
    //Symbol table object that are needed to store all symbol details for semantic analysis and code generation
    SymbolTables& m_symbolTables;
    //List to store all the output vm instructions
    std::vector<std::string> m_outputCode;
    //Used to assign unique names to any labels
    int m_labelCount;
    //Used to record the number of local variables in a subroutine, needed when defining the subroutine in the vm code
    int m_numLocalVariables;
    //Used to record the number of fields in the class, needed when allocating space in the constructor
    int m_numFieldVariables;
    //Records the index into the m_outputCode list where the amount of memory needed to be allocated is specified (in the constructor) to be adjusted later
    int m_indexOfNumOfFieldsCode;
    //List of symbols that are unresolved - should be empty by the end of compilation
    std::list<SymbolToBeResolved>& m_symbolsToBeResolved;
    //Name of the current class
    std::string m_className;
    //File path of the current file being compiled
    std::string m_filePath;
    //Records the data type that should be returned by the current subroutine being parsed
    std::string m_scopeReturnType;
    //Records whether the current block of code returns a value on all code paths
    bool m_returnsValue;
    /**
    * Removes any occurrences of the symbol passed in from the unresolvedSymbols list
    */
    void resolveSymbol(const std::string& name, const Symbol::SymbolKind& symbolKind, const std::vector<std::string>* parameterList);
    /**
    * Returns a boolean indicating whether the data type passed in is a class or not
    */
    bool isClassType(const std::string& symbolType) const;
    /**
    * Returns a boolean indicating whether the given symbol has already been declared and is found in the symbol tables
    */
    bool checkSymbolRedeclaration(const std::string& name, const Symbol::SymbolKind& symbolKind) const;
    /**
    * Resolve any of the symbols that have been defined in this class
    */
    void resolveSymbols();
    /**
    * Returns a boolean indicating whether the given symbol is still to be resolved and has not been defined
    * - if this is true then the symbol is also added to the symbolsToBeResolved list
    */
    bool determineIfNeedsToBeResolved(const std::string& symbolName, const Symbol::SymbolKind& symbolKind, std::pair<bool, std::vector<std::string>> parameterList);
    /**
    * Compares a the defined parameter list of a function to an argument list supplied in the source code and will flag any errors if
    * inconsistencies occur
    */
    void compareArgumentListToParameterList(const std::vector<std::string>* parameterList, const std::vector<std::string>& expressionListDataTypes) const;

    int getLabelCount() { return m_labelCount++; }

    /*
      All the methods that form the recursive descent parser
    */

    void jackProgram();
    void classDefinition();
    void memberDefinition();
    void classVariableDefinition();
    void subroutineDefinition();
    void type();
    const std::pair<std::vector<std::string>, std::vector<std::string>> parameterList();
    /**
    * returns a boolean indicating whether that body returns a value in all code paths
    */ 
    bool body();
    void statement();
    void variableDeclarationStatement();
    void letStatement();
    void ifStatement();
    void whileStatement();
    void doStatement();
    void returnStatement();
    /**
    * returns a data type of the expression for semantic analysis
    */ 
    std::string expression();
    void subroutineCall();
    /**
    * returns a list of data types for checking parameter lists when calling functions
    */ 
    const std::vector<std::string> expressionList();
    /**
    * returns a data type for semantic analysis
    */ 
    std::string relationalExpression();
    /**
    * returns a data type for semantic analysis
    */ 
    std::string arithmeticExpression();
    /**
    * returns a data type for semantic analysis
    */ 
    std::string term();
    /**
    * returns a data type for semantic analysis
    */ 
    std::string factor();
    /**
    * returns a data type for semantic analysis
    */ 
    std::string operand();
    /**
    * Returns a bool indicating whether the next token is part of an expression
    */ 
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