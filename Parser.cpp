#include "Parser.h"
#include <algorithm>

namespace JackCompiler
{
  void Parser::parse()
  {
    jackProgram();
  }

  void Parser::jackProgram()
  {
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
      type();
      if ((token = m_lexer.getNextToken()).m_tokenType == Token::TokenType::IDENTIFIER)
      {
        Token token;
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
      compilerError("Expected the KEYWORD 'static' or the KEYWORD 'field' at this position", m_lexer.getLineNum(), token.m_lexeme); 
    
  }

  void Parser::subroutineDefinition()
  {
    Token token = m_lexer.getNextToken();
    if (token.m_lexeme == "constructor" || token.m_lexeme == "function" || token.m_lexeme == "method")
    {
      Token nextToken = m_lexer.peekNextToken();
      if (nextToken.m_lexeme == "int" || nextToken.m_lexeme == "char" || nextToken.m_lexeme == "boolean" || token.m_tokenType == Token::TokenType::IDENTIFIER)
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
    m_lexer.getNextToken();
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
    m_lexer.getNextToken();
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