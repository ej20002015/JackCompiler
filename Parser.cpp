#include "Parser.h"
#include <algorithm>

namespace JackCompiler
{
  void Parser::parse()
  {
    jackProgram();
  }

  void Parser::resolveSymbol(const std::string& name)
  {
    for (std::list<SymbolToBeResolved>::iterator iterator = m_symbolsToBeResolved.begin(); iterator != m_symbolsToBeResolved.end(); ++iterator)
    {
      if (iterator->m_name == name)
      {
        m_symbolsToBeResolved.erase(iterator);
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
            //TODO: Check class symbol table and resolve any symbols
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
      std::string typeString = m_lexer.peekNextToken().m_lexeme;
      Symbol::SymbolType newSymbolType;
      if (typeString == "int")
        newSymbolType = Symbol::SymbolType::INT;
      else if (typeString == "char")
        newSymbolType = Symbol::SymbolType::CHAR;
      else if (typeString == "boolean")
        newSymbolType = Symbol::SymbolType::BOOLEAN;
      else
      {
        newSymbolType = Symbol::SymbolType::CLASS;
        //attempt to find this class in a previous symbol table, otherwise add it to the list to be resolved later
        if (!m_symbolTables.checkClassDefined(typeString))
          m_symbolsToBeResolved.push_back({typeString, m_lexer.getLineNum()});
      }
        
      type();
      if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
      {
        if (m_symbolTables.checkSymbolExistsInAllSymbolTables(token.m_lexeme, newSymbolKind))
          compilerError("IDENTIFIER has already been declared", m_lexer.getLineNum(), token.m_lexeme);
        //create new symbol
        m_symbolTables.addToSymbolTables(m_className + "." + token.m_lexeme, newSymbolKind, newSymbolType);

        while ((token = m_lexer.getNextToken()).m_lexeme == ",")
        {
          if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
          {
            if (m_symbolTables.checkSymbolExistsInAllSymbolTables(m_className + "." + token.m_lexeme, newSymbolKind))
              compilerError("IDENTIFIER has already been declared", m_lexer.getLineNum(), token.m_lexeme);
            m_symbolTables.addToSymbolTables(token.m_lexeme, newSymbolKind, newSymbolType);
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
      //TODO: resolve local and class symbols
      Token nextToken = m_lexer.peekNextToken();
      if (nextToken.m_lexeme == "int" || nextToken.m_lexeme == "char" || nextToken.m_lexeme == "boolean" || nextToken.m_tokenType == Token::TokenType::IDENTIFIER)
        type();
      else if ((token = m_lexer.getNextToken()).m_lexeme == "void")
      {
      }
      else
        compilerError("Expected the KEYWORD 'int', the KEYWORD 'char', the KEYWORD 'boolean', an IDENTIFIER or the KEYWORD 'void' at this position", m_lexer.getLineNum(), nextToken.m_lexeme);  
      
      if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
      {
        if ((token = m_lexer.getNextToken()).m_lexeme == "(")
        {
          parameterList();
          if ((token = m_lexer.getNextToken()).m_lexeme == ")")
            body();
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

  void Parser::parameterList()
  {
    Token nextToken = m_lexer.peekNextToken();
    if (nextToken.m_lexeme == "int" || nextToken.m_lexeme == "char" || nextToken.m_lexeme == "boolean" || nextToken.m_tokenType == Token::TokenType::IDENTIFIER)
    {
      type();
      Token token = m_lexer.getNextToken();
      if (token.m_tokenType == Token::TokenType::IDENTIFIER)
      {
      }
      else
        compilerError("Expected an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);
      while ((nextToken = m_lexer.peekNextToken()).m_lexeme == ",")
      {
        m_lexer.getNextToken();
        type();
        if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
        {
        }
        else
          compilerError("Expected an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);
      }
    }
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
      type();
      if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
      {
        while ((token = m_lexer.getNextToken()).m_lexeme == ",")
        {
          if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
          {
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
    if (token.m_lexeme == "let")
    {
      if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
      {
        Token nextToken = m_lexer.peekNextToken();
        if (nextToken.m_lexeme == "[")
        {
          m_lexer.getNextToken();
          expression();
          if ((token = m_lexer.getNextToken()).m_lexeme == "]")
          {
          }
          else
            compilerError("Expected the SYMBOL ']' at this position", m_lexer.getLineNum(), token.m_lexeme);
        }

        if ((token = m_lexer.getNextToken()).m_lexeme == "=")
        {
          expression();
          if ((token = m_lexer.getNextToken()).m_lexeme == ";")
          {
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
        expression();
      
      if ((token = m_lexer.getNextToken()).m_lexeme == ";")
      {
      }
      else
        compilerError("Expected the SYMBOL ';' at this position", m_lexer.getLineNum(), token.m_lexeme);
    }
    else
      compilerError("Expected the KEYWORD 'return' at this position", m_lexer.getLineNum(), token.m_lexeme);
  }

  void Parser::expression()
  {
    relationalExpression();
    Token nextToken = m_lexer.peekNextToken();
    while (nextToken.m_lexeme == "&" || nextToken.m_lexeme == "|")
    {
      m_lexer.getNextToken();
      relationalExpression();
      nextToken = m_lexer.peekNextToken();
    }
  }

  void Parser::subroutineCall()
  {
    Token token = m_lexer.getNextToken();
    if (token.m_tokenType == Token::TokenType::IDENTIFIER)
    {
      Token nextToken = m_lexer.peekNextToken();
      if (nextToken.m_lexeme == ".")
      {
        m_lexer.getNextToken();
        if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
        {
        }
        else
          compilerError("Expected an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);
      }

      if ((token = m_lexer.getNextToken()).m_lexeme == "(")
      {
        expressionList();
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

  void Parser::expressionList()
  {
    Token nextToken = m_lexer.peekNextToken();
    if (isExpression(nextToken))
    {
      expression();
      while ((nextToken = m_lexer.peekNextToken()).m_lexeme == ",")
      {
        m_lexer.getNextToken();
        expression();
      }
    }
  }

  void Parser::relationalExpression()
  {
    arithmeticExpression();
    Token nextToken = m_lexer.peekNextToken();
    while (nextToken.m_lexeme == "=" || nextToken.m_lexeme == ">" || nextToken.m_lexeme == "<")
    {
      m_lexer.getNextToken();
      arithmeticExpression();
      nextToken = m_lexer.peekNextToken();
    }
  }

  void Parser::arithmeticExpression()
  {
    term();
    Token nextToken = m_lexer.peekNextToken();
    while (nextToken.m_lexeme == "+" || nextToken.m_lexeme == "-")
    {
      m_lexer.getNextToken();
      term();
      nextToken = m_lexer.peekNextToken();
    }
  }

  void Parser::term()
  {
    factor();
    Token nextToken = m_lexer.peekNextToken();
    while (nextToken.m_lexeme == "*" || nextToken.m_lexeme == "/")
    {
      m_lexer.getNextToken();
      factor();
      nextToken = m_lexer.peekNextToken();
    }
  }

  void Parser::factor()
  {
    Token nextToken = m_lexer.peekNextToken();
    if (nextToken.m_lexeme == "-" || nextToken.m_lexeme == "~")
    {
      m_lexer.getNextToken();
    }
    operand();
  }

  void Parser::operand()
  {
    Token nextToken = m_lexer.peekNextToken();
    if (nextToken.m_tokenType == Token::TokenType::INTEGERCONSTANT)
    {
      m_lexer.getNextToken();
    }
    else if (nextToken.m_tokenType == Token::TokenType::IDENTIFIER)
    {
      m_lexer.getNextToken();
      if ((nextToken = m_lexer.peekNextToken()).m_lexeme == ".")
      {
        m_lexer.getNextToken();
        Token token = m_lexer.getNextToken();
        if (token.m_tokenType == Token::TokenType::IDENTIFIER)
        {
        }
        else
          compilerError("Expected an IDENTIFIER at this position", m_lexer.getLineNum(), token.m_lexeme);
      }

      nextToken = m_lexer.peekNextToken(); 
      if (nextToken.m_lexeme == "[")
      {
        m_lexer.getNextToken();
        expression();
        Token token = m_lexer.getNextToken();
        if (token.m_lexeme == "]")
        {
        }
        else
          compilerError("Expected the SYMBOL ']' at this position", m_lexer.getLineNum(), token.m_lexeme);
      }
      else if (nextToken.m_lexeme == "(")
      {
        m_lexer.getNextToken();
        expressionList();
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
      expression();
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
    }
    else if (nextToken.m_lexeme == "true")
    {
      m_lexer.getNextToken();
    }
    else if (nextToken.m_lexeme == "false")
    {
      m_lexer.getNextToken();
    }
    else if (nextToken.m_lexeme == "null")
    {
      m_lexer.getNextToken();
    }
    else if (nextToken.m_lexeme == "this")
    {
      m_lexer.getNextToken();
    }
    else
      compilerError("Expected an INTEGERCONSTANT, an IDENTIFIER, the SYMBOL '(', a STRINGCONSTANT, the KEYWORD 'true', the KEYWORD 'false', the KEYWORD 'null' or the KEYWORD 'this' at this position", m_lexer.getLineNum(), nextToken.m_lexeme);
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