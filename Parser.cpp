#include "Parser.h"
#include <algorithm>

namespace JackCompiler
{
  void Parser::parse()
  {
    jackProgram();
  }

  void Parser::resolveSymbol(const std::string& name, const Symbol::SymbolKind& symbolKind, const std::vector<std::string>* parameterList)
  {
    std::vector<Symbol::SymbolKind> functionKinds {Symbol::SymbolKind::CONSTRUCTOR, Symbol::SymbolKind::FUNCTION, Symbol::SymbolKind::METHOD};

    //use the erase-remove idiom to go through the list of unresolved symbols and delete any symbols matching the arguments passed in
    m_symbolsToBeResolved.erase(std::remove_if(m_symbolsToBeResolved.begin(),
                                              m_symbolsToBeResolved.end(),
                                              [=](const SymbolToBeResolved& symbolToBeResolved)
                                              {
                                                if (symbolToBeResolved.m_name == name && (symbolToBeResolved.m_kind == symbolKind || std::find(functionKinds.begin(), functionKinds.end(), symbolKind) != functionKinds.end()))
                                                {
                                                  //check parameter list matches
                                                  if (parameterList)
                                                  {
                                                    if (symbolToBeResolved.m_parameterList.first)
                                                    {
                                                      if (parameterList->size() != symbolToBeResolved.m_parameterList.second.size())
                                                        compilerError("Argument list is not of the correct length : " + symbolToBeResolved.m_fileName, symbolToBeResolved.m_lineNum, "(");
                                                      else
                                                      {
                                                        for (int i = 0; i < parameterList->size(); ++i)
                                                        {
                                                          if (parameterList->at(i) != symbolToBeResolved.m_parameterList.second.at(i) && symbolToBeResolved.m_parameterList.second.at(i) != "any" && parameterList->at(i) != "any" && 
                                                          !(parameterList->at(i) == "int" && symbolToBeResolved.m_parameterList.second.at(i) == "char") && !(parameterList->at(i) == "char" && symbolToBeResolved.m_parameterList.second.at(i) == "int"))
                                                            compilerError("Argument list does not match the data types of the function parameters : " + symbolToBeResolved.m_fileName, symbolToBeResolved.m_lineNum, "(");
                                                        }
                                                      }
                                                    }
                                                  }
                                                  return true; 
                                                }
                                                return false;
                                              }),
                                m_symbolsToBeResolved.end()
    );
  }

  bool Parser::isClassType(const std::string& symbolType) const
  {
    return std::find(m_primitiveDataType.begin(), m_primitiveDataType.end(), symbolType) == m_primitiveDataType.end();
  }

  bool Parser::checkSymbolRedeclaration(const std::string& name, const Symbol::SymbolKind& symbolKind) const
  {
    //if symbol is a local variable or argument then it will only conflict with variables in the local scope so only check the upmost table
    if (symbolKind == Symbol::SymbolKind::ARGUMENT || symbolKind == Symbol::SymbolKind::VAR)
      return m_symbolTables.checkSymbolExistsInCurrentSymbolTable(name, symbolKind);
    else
      return m_symbolTables.checkSymbolExistsInAllSymbolTables(name, symbolKind);
  }
  
  void Parser::resolveSymbols()
  {
    //get list of symbols that have been defined in the current class to resolve
    std::list<std::shared_ptr<Symbol>> symbolsToResolve = m_symbolTables.getSymbolsFromCurrentSymbolTable();
    for (auto symbol : symbolsToResolve)
    {
      resolveSymbol(symbol->m_name, symbol->m_kind, symbol->getParameterList());
    }

    //resolve any class references
    resolveSymbol(m_className, Symbol::SymbolKind::CLASS, nullptr);
  }

  bool Parser::determineIfNeedsToBeResolved(const std::string& symbolName, const Symbol::SymbolKind& symbolKind, std::pair<bool, std::vector<std::string>> parameterList)
  {
    if (isClassType(symbolName))
    {
      //attempt to find this class in a previous symbol table, otherwise add it to the list to be resolved later
      if (symbolName.find('.') == std::string::npos)
      {
        if (!m_symbolTables.checkClassDefined(symbolName))
        {
          m_symbolsToBeResolved.push_back({symbolName, m_filePath, m_lexer.getLineNum(), symbolKind, parameterList});
          return true;
        }
      }
      else
      {
        if (!m_symbolTables.checkSymbolExistsInAllSymbolTables(symbolName, Symbol::SymbolKind::FUNCTION))
        {
          m_symbolsToBeResolved.push_back({symbolName, m_filePath, m_lexer.getLineNum(), symbolKind, parameterList});
          return true;
        }
      }
    }
    return false;
  }

  void Parser::compareArgumentListToParameterList(const std::vector<std::string>* parameterList, const std::vector<std::string>& expressionListDataTypes) const
  {
    if (parameterList)
    {
      if (parameterList->size() != expressionListDataTypes.size())
        compilerError("Argument list is not of the correct length", m_lexer.getLineNum(), "(");
      else
      {
        for (int i = 0; i < parameterList->size(); ++i)
        {
          if (parameterList->at(i) != expressionListDataTypes.at(i) && expressionListDataTypes.at(i) != "any" && parameterList->at(i) != "any" && 
          !(parameterList->at(i) == "int" && expressionListDataTypes.at(i) == "char") && !(parameterList->at(i) == "char" && expressionListDataTypes.at(i) == "int"))
            compilerError("Argument list does not match the data types of the function parameters", m_lexer.getLineNum(), "(");
        }
      }
    }
  }

  void Parser::jackProgram()
  {
    //if file is empty
    if (m_lexer.peekNextToken().m_tokenType == Token::TokenType::EOFILE)
      return;

    classDefinition();
    Token token = m_lexer.getNextToken();
    if (token.m_tokenType == Token::TokenType::EOFILE)
    {
    }
    else
      compilerError("Expected the EOF token at this position", m_lexer.getLineNum(), token.m_lexeme);
  
  }

  void Parser::classDefinition()
  {
    Token token = m_lexer.getNextToken();
    if (token.m_lexeme == "class")
    {
      if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
      {
        //check a class has not already been declared with the same name
        if (m_symbolTables.checkClassDefined(token.m_lexeme))
          compilerError("class with the IDENTIFIER has already been defined", m_lexer.getLineNum(), token.m_lexeme);

        m_symbolTables.addSymbolTable(SymbolTable(token.m_lexeme));

        m_className = token.m_lexeme;
        if ((token = m_lexer.getNextToken()).m_lexeme == "{")
        {
          Token nextToken = m_lexer.peekNextToken();
          while (nextToken.m_lexeme == "static" || nextToken.m_lexeme == "field" || nextToken.m_lexeme == "constructor" || nextToken.m_lexeme == "function" || nextToken.m_lexeme == "method")
          {
            memberDefinition();
            nextToken = m_lexer.peekNextToken();
          }
          if ((token = m_lexer.getNextToken()).m_lexeme == "}")
          {
            resolveSymbols();
          }
          else
            compilerError("Expected the SYMBOL '}' at this position", m_lexer.getLineNum(), token.m_lexeme);
        }
        else
          compilerError("Expected the SYMBOL '{' at this position", m_lexer.getLineNum(), token.m_lexeme);
      }
      else
        compilerError("Expected an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);
    }
    else
      compilerError("Expected the KEYWORD 'class' at this position", m_lexer.getLineNum(), token.m_lexeme);    
  }

  void Parser::memberDefinition()
  {
    Token nextToken = m_lexer.peekNextToken();
    if (nextToken.m_lexeme == "static" || nextToken.m_lexeme == "field")
      classVariableDefinition();
    else if (nextToken.m_lexeme == "constructor" || nextToken.m_lexeme == "function" || nextToken.m_lexeme == "method")
      subroutineDefinition();
    else
      compilerError("Expected the KEYWORD 'static', the KEYWORD 'field', the KEYWORD 'constructor', the KEYWORD 'function' or the KEYWORD 'method' at this position", m_lexer.getLineNum(), nextToken.m_lexeme);
  }

  void Parser::classVariableDefinition()
  {
    Token token = m_lexer.getNextToken();
    if (token.m_lexeme == "static" || token.m_lexeme == "field")
    {
      Symbol::SymbolKind newSymbolKind;
      if (token.m_lexeme == "static")
        newSymbolKind = Symbol::SymbolKind::STATIC;
      else
        newSymbolKind = Symbol::SymbolKind::FIELD;
      
      //peak next token and assume it is a correct type - if it is not the compiler will error appropriately in type()
      std::string newSymbolType = m_lexer.peekNextToken().m_lexeme;
      determineIfNeedsToBeResolved(newSymbolType, Symbol::SymbolKind::CLASS, std::pair<bool, std::vector<std::string>>(false, std::vector<std::string>()));
      type();
      if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
      {
        if (checkSymbolRedeclaration(token.m_lexeme, newSymbolKind))
          compilerError("IDENTIFIER has already been declared", m_lexer.getLineNum(), token.m_lexeme);
        //create new symbol
        m_symbolTables.addToSymbolTables(m_className + "." + token.m_lexeme, newSymbolKind, newSymbolType);

        while ((token = m_lexer.getNextToken()).m_lexeme == ",")
        {
          if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
          {
            if (checkSymbolRedeclaration(m_className + "." + token.m_lexeme, newSymbolKind))
              compilerError("IDENTIFIER has already been declared", m_lexer.getLineNum(), token.m_lexeme);
            m_symbolTables.addToSymbolTables(m_className + "." + token.m_lexeme, newSymbolKind, newSymbolType);
          }
          else
            compilerError("Expected an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);
        }
        if (token.m_lexeme == ";")
        {
        }
        else
          compilerError("Expected the SYMBOL ';' at this position", m_lexer.getLineNum(), token.m_lexeme);
      }
      else
        compilerError("Expected an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);
    }
    else
      compilerError("Expected the KEYWORD 'static' or the KEYWORD 'field' at this position", m_lexer.getLineNum(), token.m_lexeme);
  }

  void Parser::subroutineDefinition()
  {
    Token token = m_lexer.getNextToken();
    if (token.m_lexeme == "constructor" || token.m_lexeme == "function" || token.m_lexeme == "method")
    {
      Symbol::SymbolKind newSymbolKind;
      if (token.m_lexeme == "constructor")
        newSymbolKind = Symbol::SymbolKind::CONSTRUCTOR;
      else if (token.m_lexeme == "function")
        newSymbolKind = Symbol::SymbolKind::FUNCTION;
      else
        newSymbolKind = Symbol::SymbolKind::METHOD;

      Token nextToken = m_lexer.peekNextToken();
      std::string newSymbolType = nextToken.m_lexeme;
      determineIfNeedsToBeResolved(newSymbolType, Symbol::SymbolKind::CLASS, std::pair<bool, std::vector<std::string>>(false, std::vector<std::string>()));

      if (nextToken.m_lexeme == "int" || nextToken.m_lexeme == "char" || nextToken.m_lexeme == "boolean" || nextToken.m_tokenType == Token::TokenType::IDENTIFIER)
        type();
      else if ((token = m_lexer.getNextToken()).m_lexeme == "void")
      {
      }
      else
        compilerError("Expected the KEYWORD 'int', the KEYWORD 'char', the KEYWORD 'boolean', an IDENTIFIER or the KEYWORD 'void' at this position", m_lexer.getLineNum(), nextToken.m_lexeme);  
      
      if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
      {
        std::string newSymbolName = m_className + "." + token.m_lexeme;
        if ((token = m_lexer.getNextToken()).m_lexeme == "(")
        {
          auto newSymbolParameterListPair = parameterList();
          std::vector<std::string> newSymbolParameterListTypes = newSymbolParameterListPair.first;
          std::vector<std::string> newSymbolParameterListNames = newSymbolParameterListPair.second;
          if ((token = m_lexer.getNextToken()).m_lexeme == ")")
          {
            //TODO: go through body statements
            //make new symbol table for this subroutine scope
            m_symbolTables.addToSymbolTables(newSymbolName, newSymbolKind, newSymbolType, newSymbolParameterListTypes);
            m_scopeReturnType = newSymbolType;
            m_symbolTables.addSymbolTable(SymbolTable());
            //add arguments to table
            for (int i = 0; i < newSymbolParameterListTypes.size(); ++i)
            {
              m_symbolTables.addToSymbolTables(newSymbolParameterListNames.at(i), Symbol::SymbolKind::ARGUMENT, newSymbolParameterListTypes.at(i));  
            }
            body();
            //remove symbol table for this subroutine scope
            m_symbolTables.removeCurrentSymbolTable();
          }
          else
            compilerError("Expected the SYMBOL ')' at this position", m_lexer.getLineNum(), token.m_lexeme);  
        }
        else
          compilerError("Expected the SYMBOL '(' at this position", m_lexer.getLineNum(), token.m_lexeme);
      }
      else
        compilerError("Expected an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);
    }
    else
      compilerError("Expected the KEYWORD 'constructor', the KEYWORD 'function' or the KEYWORD 'method' at this position", m_lexer.getLineNum(), token.m_lexeme);
  }

  void Parser::type()
  {
    Token token = m_lexer.getNextToken();
    if (token.m_lexeme == "int" || token.m_lexeme == "char" || token.m_lexeme == "boolean" || token.m_tokenType == Token::TokenType::IDENTIFIER)
    {
    }
    else
      compilerError("Expected the KEYWORD 'int', the KEYWORD 'char', the KEYWORD 'boolean' or an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);
  }

  const std::pair<std::vector<std::string>, std::vector<std::string>> Parser::parameterList()
  {
    //list of data types in the parameter list. returned to the calling function to create a new FunctionSymbol
    std::vector<std::string> parameterListTypes;
    std::vector<std::string> parameterListNames;
    Token nextToken = m_lexer.peekNextToken();

    if (nextToken.m_lexeme == "int" || nextToken.m_lexeme == "char" || nextToken.m_lexeme == "boolean" || nextToken.m_tokenType == Token::TokenType::IDENTIFIER)
    {
      std::string parameterType = nextToken.m_lexeme;
      determineIfNeedsToBeResolved(parameterType, Symbol::SymbolKind::CLASS, std::pair<bool, std::vector<std::string>>(false, std::vector<std::string>()));

      parameterListTypes.push_back(parameterType);

      type();
      Token token = m_lexer.getNextToken();
      if (token.m_tokenType == Token::TokenType::IDENTIFIER)
      {
        parameterListNames.push_back(token.m_lexeme);
      }
      else
        compilerError("Expected an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);
      while ((nextToken = m_lexer.peekNextToken()).m_lexeme == ",")
      {
        m_lexer.getNextToken();

        nextToken = m_lexer.peekNextToken();
        std::string parameterType = nextToken.m_lexeme;
        determineIfNeedsToBeResolved(parameterType, Symbol::SymbolKind::CLASS, std::pair<bool, std::vector<std::string>>(false, std::vector<std::string>()));

        parameterListTypes.push_back(parameterType);

        type();
        if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
        {
          parameterListNames.push_back(token.m_lexeme);
        }
        else
          compilerError("Expected an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);
      }
    }

    return std::pair<std::vector<std::string>, std::vector<std::string>>{parameterListTypes, parameterListNames};
  }

  void Parser::body()
  {
    Token token = m_lexer.getNextToken();
    if (token.m_lexeme == "{")
    {
      Token nextToken = m_lexer.peekNextToken();
      while (nextToken.m_lexeme == "var" || nextToken.m_lexeme == "let" || nextToken.m_lexeme == "if" || nextToken.m_lexeme == "while" || nextToken.m_lexeme == "do" || nextToken.m_lexeme == "return")
      {
        statement();
        nextToken = m_lexer.peekNextToken();
      }
      if ((token = m_lexer.getNextToken()).m_lexeme == "}")
      {
      }
      else
        compilerError("Expected the SYMBOL '}' at this position", m_lexer.getLineNum(), token.m_lexeme);  
    }
    else
      compilerError("Expected the SYMBOL '{' at this position", m_lexer.getLineNum(), token.m_lexeme);
  }

  void Parser::statement()
  {
    Token nextToken = m_lexer.peekNextToken();
    if (nextToken.m_lexeme == "var")
      variableDeclarationStatement();
    else if (nextToken.m_lexeme == "let")
      letStatement();
    else if (nextToken.m_lexeme == "if")
      ifStatement();
    else if (nextToken.m_lexeme == "while")
      whileStatement();
    else if (nextToken.m_lexeme == "do")
      doStatement();
    else if (nextToken.m_lexeme == "return")
      returnStatement();
    else
      compilerError("Expected the KEYWORD 'var', the KEYWORD 'let', the KEYWORD 'if', the KEYWORD 'while', the KEYWORD 'do' or the KEYWORD 'return' at this position", m_lexer.getLineNum(), nextToken.m_lexeme);
  }

  void Parser::variableDeclarationStatement()
  {
    Token token = m_lexer.getNextToken();
    if (token.m_lexeme == "var")
    {
      std::string newSymbolType = m_lexer.peekNextToken().m_lexeme;
      determineIfNeedsToBeResolved(newSymbolType, Symbol::SymbolKind::CLASS, std::pair<bool, std::vector<std::string>>(false, std::vector<std::string>()));
      type();
      if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
      {
        if (checkSymbolRedeclaration(token.m_lexeme, Symbol::SymbolKind::VAR))
          compilerError("IDENTIFIER has already been declared", m_lexer.getLineNum(), token.m_lexeme);
        
        //add token to subroutine scope symbol table
        m_symbolTables.addToSymbolTables(token.m_lexeme, Symbol::SymbolKind::VAR, newSymbolType);

        while ((token = m_lexer.getNextToken()).m_lexeme == ",")
        {
          if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
          {
            if (checkSymbolRedeclaration(token.m_lexeme, Symbol::SymbolKind::VAR))
              compilerError("IDENTIFIER has already been declared", m_lexer.getLineNum(), token.m_lexeme);

            m_symbolTables.addToSymbolTables(token.m_lexeme, Symbol::SymbolKind::VAR, newSymbolType);
          }
          else
            compilerError("Expected an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);    
        }
        if (token.m_lexeme == ";")
        {
        }
        else
          compilerError("Expected the SYMBOL ';' at this position", m_lexer.getLineNum(), token.m_lexeme);
      }
      else
        compilerError("Expected an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);
    }
    else
      compilerError("Expected the KEYWORD 'var' at this position", m_lexer.getLineNum(), token.m_lexeme);
  }

  void Parser::letStatement()
  {
    Token token = m_lexer.getNextToken();
    std::string leftHandSideType;
    if (token.m_lexeme == "let")
    {
      if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
      {
        std::string symbolName = token.m_lexeme;
        if (!m_symbolTables.checkSymbolExistsInAllSymbolTables(symbolName, Symbol::SymbolKind::ARGUMENT) && !m_symbolTables.checkSymbolExistsInAllSymbolTables(m_className + "." + symbolName, Symbol::SymbolKind::FIELD))
          compilerError("IDENTIFIER has not been declared", m_lexer.getLineNum(), symbolName);

        Token nextToken = m_lexer.peekNextToken();
        if (nextToken.m_lexeme == "[")
        {
          m_lexer.getNextToken();
          std::string expressionType = expression();
          if (expressionType != "int" && expressionType != "any")
            compilerError("Expression in brackets does not evaluate as an INT", m_lexer.getLineNum(), "[");

          if ((token = m_lexer.getNextToken()).m_lexeme == "]")
          {
            leftHandSideType = "any";
          }
          else
            compilerError("Expected the SYMBOL ']' at this position", m_lexer.getLineNum(), token.m_lexeme);
        }

        if ((token = m_lexer.getNextToken()).m_lexeme == "=")
        {
          std::string expressionType = expression();
          auto symbolTypePair = m_symbolTables.getSymbolType(symbolName, m_className);
          if (leftHandSideType != "any")
            leftHandSideType = symbolTypePair.second;
          if (symbolTypePair.first != true || (leftHandSideType != expressionType && (!(leftHandSideType == "char" && expressionType == "int") && !(leftHandSideType == "int" && expressionType == "char")) && expressionType != "any") && leftHandSideType != "any")
            compilerError("Expression on the right hand side of the assignment does not match the data type of the variable", m_lexer.getLineNum(), symbolName);

          if ((token = m_lexer.getNextToken()).m_lexeme == ";")
          {
            //declare the symbol as being initialised with a value
            m_symbolTables.setSymbolInitialised(symbolName, m_className);
          }
          else
            compilerError("Expected the SYMBOL ';' at this position", m_lexer.getLineNum(), token.m_lexeme);
        }
        else
          compilerError("Expected the SYMBOL '=' at this position", m_lexer.getLineNum(), token.m_lexeme);
      }
      else
        compilerError("Expected an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);
    }
    else
      compilerError("Expected the KEYWORD 'let' at this position", m_lexer.getLineNum(), token.m_lexeme);
  }

  void Parser::ifStatement()
  {
    Token token = m_lexer.getNextToken();
    if (token.m_lexeme == "if")
    {
      if ((token = m_lexer.getNextToken()).m_lexeme == "(")
      {
        expression();
        if ((token = m_lexer.getNextToken()).m_lexeme == ")")
        {
          body();
          Token nextToken = m_lexer.peekNextToken();
          if (nextToken.m_lexeme == "else")
          {
            m_lexer.getNextToken();
            body();
          }
        }
        else
          compilerError("Expected the SYMBOL ')' at this position", m_lexer.getLineNum(), token.m_lexeme);
      }
      else
        compilerError("Expected the SYMBOL '(' at this position", m_lexer.getLineNum(), token.m_lexeme);
    }
    else
      compilerError("Expected the KEYWORD 'if' at this position", m_lexer.getLineNum(), token.m_lexeme);
  }

  void Parser::whileStatement()
  {
    Token token = m_lexer.getNextToken();
    if (token.m_lexeme == "while")
    {
      if ((token = m_lexer.getNextToken()).m_lexeme == "(")
      {
        expression();
        if ((token = m_lexer.getNextToken()).m_lexeme == ")")
          body();
        else
          compilerError("Expected the SYMBOL ')' at this position", m_lexer.getLineNum(), token.m_lexeme);
      }
      else
        compilerError("Expected the SYMBOL '(' at this position", m_lexer.getLineNum(), token.m_lexeme);
    }
    else
      compilerError("Expected the KEYWORD 'if' at this position", m_lexer.getLineNum(), token.m_lexeme);
  }

  void Parser::doStatement()
  {
    Token token = m_lexer.getNextToken();
    if (token.m_lexeme == "do")
    {
      subroutineCall();
      if ((token = m_lexer.getNextToken()).m_lexeme == ";")
      {
      }
      else
        compilerError("Expected the SYMBOL ';' at this position", m_lexer.getLineNum(), token.m_lexeme);
    }
    else
      compilerError("Expected the KEYWORD 'do' at this position", m_lexer.getLineNum(), token.m_lexeme);
  }

  void Parser::returnStatement()
  {
    Token token = m_lexer.getNextToken();
    if (token.m_lexeme == "return")
    {
      Token nextToken = m_lexer.peekNextToken();
      if (isExpression(nextToken))
      {
        std::string returnedDataType = expression();

        if (returnedDataType != m_scopeReturnType && returnedDataType != "any" && !(returnedDataType == "int" && m_scopeReturnType == "char"))
          compilerError("Expected return value to be of type " + m_scopeReturnType + " not " + returnedDataType, m_lexer.getLineNum(), nextToken.m_lexeme);
      }
      else
      {
        if (m_scopeReturnType != "void")
          compilerError("Expected subroutine to return a value of type " + m_scopeReturnType, m_lexer.getLineNum(), nextToken.m_lexeme);
      }
      
      if ((token = m_lexer.getNextToken()).m_lexeme == ";")
      {
      }
      else
        compilerError("Expected the SYMBOL ';' at this position", m_lexer.getLineNum(), token.m_lexeme);
    }
    else
      compilerError("Expected the KEYWORD 'return' at this position", m_lexer.getLineNum(), token.m_lexeme);
  }

  std::string Parser::expression()
  {
    std::string expressionType;
    expressionType = relationalExpression();
    Token nextToken = m_lexer.peekNextToken();
    while (nextToken.m_lexeme == "&" || nextToken.m_lexeme == "|")
    {
      expressionType = "boolean";
      m_lexer.getNextToken();
      relationalExpression();
      nextToken = m_lexer.peekNextToken();
    }

    return expressionType;
  }

  void Parser::subroutineCall()
  {
    Token token = m_lexer.getNextToken();
    if (token.m_tokenType == Token::TokenType::IDENTIFIER)
    {
      std::string functionName = token.m_lexeme;
      std::string prefixFunctionName = functionName;
      Token nextToken = m_lexer.peekNextToken();
      if (nextToken.m_lexeme == ".")
      {
        m_lexer.getNextToken();
        if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
        {

          functionName = functionName + "." + token.m_lexeme;

          auto operandTypePair = m_symbolTables.getSymbolType(prefixFunctionName, m_className);
          bool found = operandTypePair.first;
          if (found)
            functionName = operandTypePair.second + "." + token.m_lexeme;
          
          operandTypePair = m_symbolTables.getSymbolType(functionName);

          if (m_symbolTables.getSymbolType(operandTypePair.second).first && !m_symbolTables.checkSymbolExistsInAllSymbolTables(functionName, Symbol::SymbolKind::FUNCTION))
          {
            compilerError("IDENTIFIER has not been declared", m_lexer.getLineNum(), prefixFunctionName + "." + token.m_lexeme);
          }
        }
        else
          compilerError("Expected an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);
      }
      else
      {
        functionName = m_className + "." + functionName;
      }
      
      const std::vector<std::string>* parameterList = m_symbolTables.getParameterList(functionName);

      if ((token = m_lexer.getNextToken()).m_lexeme == "(")
      {
        const std::vector<std::string> expressionListDataTypes = expressionList();
        determineIfNeedsToBeResolved(functionName, Symbol::SymbolKind::FUNCTION, std::pair<bool, std::vector<std::string>>(true, expressionListDataTypes));

        //Compare the expression list against the parameter list
        compareArgumentListToParameterList(parameterList, expressionListDataTypes);

        if ((token = m_lexer.getNextToken()).m_lexeme == ")")
        {
        }
        else
          compilerError("Expected the SYMBOL ')' at this position", m_lexer.getLineNum(), token.m_lexeme);
      }
      else
        compilerError("Expected the SYMBOL '(' at this position", m_lexer.getLineNum(), token.m_lexeme);
    }
    else
      compilerError("Expected an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);
  }

  const std::vector<std::string> Parser::expressionList()
  {
    std::vector<std::string> expressionListDataTypes;
    Token nextToken = m_lexer.peekNextToken();
    if (isExpression(nextToken))
    {
      expressionListDataTypes.push_back(expression());
      while ((nextToken = m_lexer.peekNextToken()).m_lexeme == ",")
      {
        m_lexer.getNextToken();
        expressionListDataTypes.push_back(expression());
      }
    }
    
    return expressionListDataTypes;
  }

  std::string Parser::relationalExpression()
  {
    std::string relationalExpressionType;
    relationalExpressionType = arithmeticExpression();
    Token nextToken = m_lexer.peekNextToken();
    while (nextToken.m_lexeme == "=" || nextToken.m_lexeme == ">" || nextToken.m_lexeme == "<")
    {
      relationalExpressionType = "boolean";
      m_lexer.getNextToken();
      arithmeticExpression();
      nextToken = m_lexer.peekNextToken();
    }

    return relationalExpressionType;
  }

  std::string Parser::arithmeticExpression()
  {
    std::string arithmeticExpressionType;
    arithmeticExpressionType = term();
    Token nextToken = m_lexer.peekNextToken();
    while (nextToken.m_lexeme == "+" || nextToken.m_lexeme == "-")
    {
      arithmeticExpressionType = "int";
      m_lexer.getNextToken();
      term();
      nextToken = m_lexer.peekNextToken();
    }

    return arithmeticExpressionType;
  }

  std::string Parser::term()
  {
    std::string termType;
    termType = factor();
    Token nextToken = m_lexer.peekNextToken();
    while (nextToken.m_lexeme == "*" || nextToken.m_lexeme == "/")
    {
      termType = "int";
      m_lexer.getNextToken();
      factor();
      nextToken = m_lexer.peekNextToken();
    }

    return termType;
  }

  std::string Parser::factor()
  {
    std::string factorType;
    Token nextToken = m_lexer.peekNextToken();
    if (nextToken.m_lexeme == "-" || nextToken.m_lexeme == "~")
    {
      m_lexer.getNextToken();
    }
    factorType = operand();

    return factorType;
  }

  std::string Parser::operand()
  {
    std::string operandType;
    Token nextToken = m_lexer.peekNextToken();
    if (nextToken.m_tokenType == Token::TokenType::INTEGERCONSTANT)
    {
      operandType = "int";
      m_lexer.getNextToken();
    }
    else if (nextToken.m_tokenType == Token::TokenType::IDENTIFIER)
    {
      std::string symbolName = nextToken.m_lexeme;
      std::string prefixSymbolName = nextToken.m_lexeme;
      auto operandTypePair = m_symbolTables.getSymbolType(symbolName, m_className);
      bool found = operandTypePair.first;
      if (!found && !isClassType(operandTypePair.second))
        compilerError("IDENTIFIER has not been declared", m_lexer.getLineNum(), symbolName);

      operandType = operandTypePair.second;

      m_lexer.getNextToken();
      if ((nextToken = m_lexer.peekNextToken()).m_lexeme == ".")
      {
        m_lexer.getNextToken();
        Token token = m_lexer.getNextToken();
        if (token.m_tokenType == Token::TokenType::IDENTIFIER)
        {
          symbolName = symbolName + "." + token.m_lexeme;

          operandTypePair = m_symbolTables.getSymbolType(prefixSymbolName, m_className);
          found = operandTypePair.first;
          if (found)
          {
            symbolName = operandTypePair.second + "." + token.m_lexeme;
          }
          
          operandTypePair = m_symbolTables.getSymbolType(symbolName);
          operandType = operandTypePair.first ? operandTypePair.second : "any";

          if (m_symbolTables.getSymbolType(operandTypePair.second).first && !m_symbolTables.checkSymbolExistsInAllSymbolTables(symbolName, Symbol::SymbolKind::FUNCTION))
          {
            compilerError("IDENTIFIER has not been declared", m_lexer.getLineNum(), prefixSymbolName + "." + token.m_lexeme);
          }
        }
        else
          compilerError("Expected an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);
      }

      if (symbolName.find('.') == std::string::npos && !m_symbolTables.getSymbolType(symbolName, m_className).first)
        compilerError("IDENTIFIER has not been declared", m_lexer.getLineNum(), symbolName);

      if (!m_symbolTables.checkSymbolInitialised(symbolName, m_className) && symbolName.find('.') == std::string::npos)
        compilerError("Symbol has not been initialised", m_lexer.getLineNum(), symbolName);

      nextToken = m_lexer.peekNextToken(); 
      if (nextToken.m_lexeme == "[")
      {
        m_lexer.getNextToken();
        std::string bracketDataType = expression();
        if (bracketDataType != "int" && bracketDataType != "any")
          compilerError("Expression in brackets does not evaluate as an INT", m_lexer.getLineNum(), "[");
        
        operandType = "any";

        Token token = m_lexer.getNextToken();
        if (token.m_lexeme == "]")
        {
        }
        else
          compilerError("Expected the SYMBOL ']' at this position", m_lexer.getLineNum(), token.m_lexeme);
      }
      else if (nextToken.m_lexeme == "(")
      {
        if (symbolName.find(".") == std::string::npos)
          symbolName = m_className + "." + symbolName;

        const std::vector<std::string>* parameterList = m_symbolTables.getParameterList(symbolName);

        m_lexer.getNextToken();
        auto expressionListDataTypes = expressionList();

        determineIfNeedsToBeResolved(symbolName, Symbol::SymbolKind::FUNCTION, std::pair<bool, std::vector<std::string>>(true, expressionListDataTypes));

        //Compare the expression list against the parameter list
        compareArgumentListToParameterList(parameterList, expressionListDataTypes);
        
        Token token = m_lexer.getNextToken();
        if (token.m_lexeme == ")")
        {
        }
        else
          compilerError("Expected the SYMBOL ')' at this position", m_lexer.getLineNum(), token.m_lexeme);
      }
    }
    else if (nextToken.m_lexeme == "(")
    {
      m_lexer.getNextToken();
      operandType = expression();
      Token token = m_lexer.getNextToken();
      if (token.m_lexeme == ")")
      {
      }
      else
        compilerError("Expected the SYMBOL ')' at this position", m_lexer.getLineNum(), token.m_lexeme);
    }
    else if (nextToken.m_tokenType == Token::TokenType::STRINGCONSTANT)
    {
      m_lexer.getNextToken();
      operandType = "String";
    }
    else if (nextToken.m_lexeme == "true")
    {
      m_lexer.getNextToken();
      operandType = "boolean";
    }
    else if (nextToken.m_lexeme == "false")
    {
      m_lexer.getNextToken();
      operandType = "boolean";
    }
    else if (nextToken.m_lexeme == "null")
    {
      m_lexer.getNextToken();
      operandType = "any";
    }
    else if (nextToken.m_lexeme == "this")
    {
      m_lexer.getNextToken();
      operandType = m_className;
    }
    else
      compilerError("Expected an INTEGERCONSTANT, an IDENTIFIER, the SYMBOL '(', a STRINGCONSTANT, the KEYWORD 'true', the KEYWORD 'false', the KEYWORD 'null' or the KEYWORD 'this' at this position", m_lexer.getLineNum(), nextToken.m_lexeme);

    return operandType;
  }

  bool Parser::isExpression(const Token& token)
  {
    if (std::find(m_possibleStartTokensOfExpression.begin(), m_possibleStartTokensOfExpression.end(), token.m_lexeme) != m_possibleStartTokensOfExpression.end())
      return true;
    else if (token.m_tokenType == Token::TokenType::INTEGERCONSTANT || token.m_tokenType == Token::TokenType::IDENTIFIER || token.m_tokenType == Token::TokenType::STRINGCONSTANT)
      return true;
    else
     return false;
  }
}