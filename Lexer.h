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
		virtual Token peakNextToken() = 0;
	};

	class Lexer : public LexerInterface
	{
	public:
		Lexer(std::string& filePath) : m_lineNum(1), m_fileStream(filePath, std::fstream::in | std::ios_base::binary), m_cachedNextToken() {}
		~Lexer() { m_fileStream.close(); }
		Token getNextToken() override;
		Token peakNextToken() override;

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
		//cache the next token when calling peak to improve 
		//performance if multiple peak calls are made successively
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