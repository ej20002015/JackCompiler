#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "Core.h"

namespace JackCompiler
{
	class LexerInterface
	{
	public:
		virtual Token getNextToken() = 0;
		virtual Token peekNextToken() = 0;
	};

	class Lexer : public LexerInterface
	{
	public:
		Lexer(const std::string& filePath) : m_lineNum(1), m_fileStream(filePath, std::fstream::in | std::ios_base::binary), m_cachedNextToken() {}
		~Lexer() { m_fileStream.close(); }
		Token getNextToken() override;
		Token peekNextToken() override;
		unsigned getLineNum() const { return m_lineNum; }

	private:
		void consumeWhiteSpace();
		bool consumeComments();
		bool consumeLineComments();
		bool consumeUntilClosingComments();
		bool checkIdentifierOrKeyword(Token& token);
		bool checkIntegerConstant(Token& token);
		bool checkStringConstant(Token& token);
		bool checkSymbol(Token& token);
		unsigned m_lineNum;
		std::fstream m_fileStream;
		//cache the next token when calling peek to improve 
		//performance if multiple peek calls are made successively
		Token m_cachedNextToken;

		const std::vector<std::string> m_keywords
		{
			"class",
			"constructor",
			"method",
			"function",
			"int",
			"boolean",
			"char",
			"void",
			"var",
			"static",
			"field",
			"let",
			"do",
			"if",
			"else",
			"while",
			"return",
			"true",
			"false",
			"null",
			"this"
		};

		const std::vector<char> m_symbols
		{
			'(',
			')',
			'[',
			']',
			'{',
			'}',
			',',
			';',
			'=',
			'.',
			'+',
			'-',
			'*',
			'/',
			'&',
			'|',
			'~',
			'<',
			'>'
		};
	};
}