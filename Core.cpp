#include "Core.h"

namespace JackCompiler
{
  void compilerError(const std::string& message)
	{
		std::cout << "COMPILATION ERROR: " << message << " - ending compilation" << std::endl;
		exit(1);
	}

	void compilerError(const std::string& message, int lineNum)
	{
		std::cout << "COMPILATION ERROR: (LINE " << lineNum << ") " << message << " - ending compilation" << std::endl;
		exit(1);
	}

	void compilerError(const std::string& message, unsigned lineNum, const std::string& lexeme)
	{
		std::cout << "COMPILATION ERROR: (LINE " << lineNum << ") (AT TOKEN '" << lexeme << "') " << message << " - ending compilation" << std::endl;
		exit(1);
	}

  void compilerWarning(const std::string& message, unsigned lineNum, const std::string& lexeme)
  {
    std::cout << "COMPILATION WARNING: (LINE " << lineNum << ") (AT TOKEN '" << lexeme << "') " << message << std::endl;
  }

  Token::Token() : m_tokenType(TokenType::NONE), m_lexeme("")
  {
    //Initialise the mapping between the elements in the enum and their string representation
    m_tokenTypeMapping[TokenType::NONE]            = "NONE";
    m_tokenTypeMapping[TokenType::EOFILE]          = "EOF";
    m_tokenTypeMapping[TokenType::SYMBOL]          = "SYMBOL";
    m_tokenTypeMapping[TokenType::KEYWORD]         = "KEYWORD";
    m_tokenTypeMapping[TokenType::INTEGERCONSTANT] = "INTEGERCONSTANT";
    m_tokenTypeMapping[TokenType::STRINGCONSTANT]  = "STRINGCONSTANT";
    m_tokenTypeMapping[TokenType::IDENTIFIER]      = "IDENTIFIER";
  };
}