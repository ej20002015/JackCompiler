#pragma once

#include <iostream>
#include <map>

#define COMPILEERROR(str) \
	std::cout << "COMPILATION ERROR: " << str << " - ending compilation" << std::endl;

namespace JackCompiler
{
	struct Token
	{
		enum class TokenType
		{
			NONE,
			EOFILE,
			SYMBOL,
			KEYWORD,
			INTEGERCONSTANT,
			STRINGCONSTANT,
			IDENTIFIER
		};

		Token() : m_tokenType(TokenType::NONE), m_lexeme("")
		{
			m_tokenTypeMapping[TokenType::NONE]            = "NONE";
			m_tokenTypeMapping[TokenType::EOFILE]          = "EOF";
			m_tokenTypeMapping[TokenType::SYMBOL]          = "SYMBOL";
			m_tokenTypeMapping[TokenType::KEYWORD]         = "KEYWORD";
			m_tokenTypeMapping[TokenType::INTEGERCONSTANT] = "INTEGERCONSTANT";
			m_tokenTypeMapping[TokenType::STRINGCONSTANT]  = "STRINGCONSTANT";
			m_tokenTypeMapping[TokenType::IDENTIFIER]      = "IDENTIFIER";
		};

		TokenType m_tokenType;
		std::string m_lexeme;
		std::map<TokenType, std::string> m_tokenTypeMapping;
	};

	inline std::ostream& operator << (std::ostream& out, Token const& token) 
	{
		out << "<" << token.m_lexeme << ", " << token.m_tokenTypeMapping.at(token.m_tokenType) << ">";
		return out;
	}
}