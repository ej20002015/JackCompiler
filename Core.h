#pragma once

#include <iostream>
#include <map>

namespace JackCompiler
{
	void compilerError(std::string message);
	void compilerError(std::string message, int lineNum);

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

		Token();
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