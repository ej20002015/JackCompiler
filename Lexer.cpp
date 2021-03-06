#include "Lexer.h"

#include <iostream>
#include <cctype>
#include <algorithm>

namespace JackCompiler
{
	Token Lexer::getNextToken()
	{
		Token token;

		int currentLineNum = -1;

		//if next token has already been peeked then the m_lineNum will have already been updated so keep it constant
		if (m_cachedNextToken.m_tokenType != Token::TokenType::NONE)
			currentLineNum = m_lineNum;

		//getting a new token so set the value of cachedNextToken to show it is no longer valid
		m_cachedNextToken.m_tokenType = Token::TokenType::NONE;

		//Consume the leading whitespace
		consumeWhiteSpace();
		//Move file pointer to start of the next token
		while (consumeComments())
			consumeWhiteSpace();

		char nextChar = m_fileStream.peek();

		//check if EOF marker has been reached
		if (nextChar == EOF)
		{
			token.m_tokenType = Token::TokenType::EOFILE;
			return token;
		}

		//check if token is an identifier or keyword
		if (checkIdentifierOrKeyword(token))
			;
		//check if token is an integer constant
		else if (checkIntegerConstant(token))
			;
		//check if token is a string constant
		else if (checkStringConstant(token))
			;
		//check if token is a symbol
		else if (checkSymbol(token))
			;
		else
			compilerError("Invalid token beginning with '" + std::string(1, m_fileStream.peek()) + "'");
		
		if (currentLineNum != -1)
			m_lineNum = currentLineNum;
		return token;
	}

	Token Lexer::peekNextToken()
	{
    //If the next token has been cached by a previous call to peekNextToken then immediately return that
		if (m_cachedNextToken.m_tokenType != Token::TokenType::NONE)
			return m_cachedNextToken;
    
    //Call get next token but make sure the file stream reader doesn't progress through the file. 
    //Then cache the peeked token so it can be quickly returned if any consecutive calls to peek are made
		int currentFilePointer = m_fileStream.tellg();
		Token token = getNextToken();
		m_fileStream.seekg(currentFilePointer);
		m_cachedNextToken = token;
		return token;
	}

	void Lexer::consumeWhiteSpace()
	{
		char nextChar = m_fileStream.peek();
		for (; nextChar != EOF && std::isspace(nextChar); m_fileStream.get(), nextChar = m_fileStream.peek())
		{
			if (nextChar == '\n')
				m_lineNum++;
		}
	}

	bool Lexer::consumeComments()
	{
		if (consumeLineComments())
			return true;
		
		if (consumeUntilClosingComments())
			return true;

		return false;
	}

	bool Lexer::consumeLineComments()
	{
		bool consumedComment = false;
		int currentFilePointer = m_fileStream.tellg();

		//consume line comments
		char nextChar = m_fileStream.peek();
		int state = 0;
		while (state == 0 || state == 1)
		{
			switch (state)
			{
			case 0:
				switch (nextChar)
				{
				case '/':
					state = 1;
					break;
				default:
					state = 3;
					break;
				}
				break;
			case 1:
				switch (nextChar)
				{
				case '/':
					state = 2;
					break;
				default:
					state = 3;
					break;
				}
				break;
			default:
				break;
			}
			//consume character
			m_fileStream.get();
			//inspect next character
			nextChar = m_fileStream.peek();
		}

		if (state == 2)
		{
			//consume characters up to new line
			for (nextChar = m_fileStream.peek(); (nextChar != EOF) && (nextChar != '\n'); m_fileStream.get(), nextChar = m_fileStream.peek());
			if (nextChar == '\n')
			{
				consumedComment = true;
				m_lineNum++;
				m_fileStream.get();
			}
		}
		else
			m_fileStream.seekg(currentFilePointer);

		return consumedComment;
	}

	bool Lexer::consumeUntilClosingComments()
	{
		bool consumedComment = false;
		int currentFilePointer = m_fileStream.tellg();

		//number to store the line where the oppening comment /* is for error reporting purposes
		int openingCommentLine;

		//Consume explicitly delimited comments
		char nextChar = m_fileStream.peek();
		int state = 0;
		while (state < 4)
		{
			switch (state)
			{
			case 0:
				switch (nextChar)
				{
				case '/':
					state = 1;
					break;
				default:
					state = 6;
					break;
				}
				break;
			case 1:
				switch (nextChar)
				{
				case '*':
					state = 2;
					openingCommentLine = m_lineNum;
					break;
				default:
					state = 6;
					break;
				}
				break;
			case 2:
				switch (nextChar)
				{
				case '*':
					state = 3;
					break;
				case EOF:
					state = 5;
					break;
				case '\n':
					m_lineNum++;
					break;
				default:
					break;
				}
				break;
			case 3:
				switch (nextChar)
				{
				case '/':
					state = 4;
					break;
				default:
					state = 2;
					break;
				}
				break;
			default:
				break;
			}
			//consume character as long as it is not EOF
			if (state != 5)
			{
				m_fileStream.get();
				//inspect next character
				nextChar = m_fileStream.peek();
			}
		}

		if (state == 4)
			consumedComment = true;
		else if (state == 5)
			compilerError("No matching ending comment token exists for the opening comment token on this line");
		else
			m_fileStream.seekg(currentFilePointer);

		return consumedComment;
	}

	bool Lexer::checkIdentifierOrKeyword(Token& token)
	{
		bool identifierOrKeyword = false;

		char nextChar = m_fileStream.peek();
		//string to store consumed characters
		std::string consumedString = "";
		int state = 0;
		while (state < 2)
		{
			switch (state)
			{
			case 0:
				switch (nextChar)
				{
				case '_':
					state = 1;
					break;
				default:
					state = isalpha(nextChar) ? 1 : 3;
					break;
				}
				break;
			case 1:
				switch (nextChar)
				{
				case '_':
					break;
				default:
					state = isalpha(nextChar) || isdigit(nextChar) ? 1 : 2;
					break;
				}
				break;
			default:
				break;
			}

			if (state == 1)
			{
				//consume character and add it to the string
				consumedString += (char)m_fileStream.get();
				//inspect next character
				nextChar = m_fileStream.peek();
			}
		}

		if (state != 3)
		{
			identifierOrKeyword = true;
			token.m_lexeme = consumedString;
			//check if consumed string is an identifier or a keyword
			if (std::find(m_keywords.begin(), m_keywords.end(), consumedString) != m_keywords.end())
				token.m_tokenType = Token::TokenType::KEYWORD;
			else
				token.m_tokenType = Token::TokenType::IDENTIFIER;
		}

		return identifierOrKeyword;
	}
	
	bool Lexer::checkIntegerConstant(Token& token)
	{
		bool integerConstant = false;

		char nextChar = m_fileStream.peek();
		//string to store consumed characters
		std::string consumedString = "";
		int state = 0;
		while (state < 2)
		{
			switch (state)
			{
			case 0:
				switch (nextChar)
				{
				default:
					state = std::isdigit(nextChar) ? 1 : 3;
					break;
				}
				break;
			case 1:
				switch (nextChar)
				{
				default:
					state = std::isdigit(nextChar) ? 1 : 2;
					break;
				}
				break;
			default:
				break;
			}
			if (state == 1)
			{
				//consume character and add it to the string
				consumedString += (char)m_fileStream.get();
				//inspect next character
				nextChar = m_fileStream.peek();
			}
		}

		if (state != 3)
		{
			integerConstant = true;
			token.m_lexeme = consumedString;
			token.m_tokenType = Token::TokenType::INTEGERCONSTANT;
		}

		return integerConstant;
	}

	bool Lexer::checkStringConstant(Token& token)
	{
		bool stringConstant = false;

		char nextChar = m_fileStream.peek();
		//string to store consumed characters
		std::string consumedString = "";
		int state = 0;
		while (state < 2)
		{
			switch (state)
			{
			case 0:
				switch (nextChar)
				{
				case '"':
					state = 1;
					break;
				default:
					state = 4;
					break;
				}
				break;
			case 1:
				switch (nextChar)
				{
				case '"':
					state = 5;
					break;
				case '\n':
					state = 3;
					break;
				case EOF:
					state = 2;
				default:
					break;
				}
				break;
			default:
				break;
			}
			if (state == 1)
			{
				//consume character and add it to the string
				consumedString += (char)m_fileStream.get();
				//inspect next character
				nextChar = m_fileStream.peek();
			}
		}

		if (state == 2)
			compilerError("No terminating \" for string constant");
		else if (state == 3)
			compilerError("New line characters are not permitted in string constants");
		else if (state == 5)
		{
			//consume the ending "
			m_fileStream.get();
			stringConstant = true;
			token.m_lexeme = consumedString + '"';
			token.m_tokenType = Token::TokenType::STRINGCONSTANT;
		}

		return stringConstant;
	}

	bool Lexer::checkSymbol(Token& token)
	{
		bool symbol = false;

		char nextChar = m_fileStream.peek();
		//string to store consumed characters
		std::string consumedString = "";
		int state = 0;
		while (state == 0)
		{
			switch (state)
			{
			case 0:
				switch (nextChar)
				{
				default:
					state = std::find(m_symbols.begin(), m_symbols.end(), nextChar) != m_symbols.end() ? 1 : 2;
					break;
				}
				break;
			default:
				break;
			}
			if (state == 1)
			{
				//consume character and add it to the string
				consumedString += (char)m_fileStream.get();
				//inspect next character
				nextChar = m_fileStream.peek();
			}
		}

		if (state == 1)
		{
			symbol = true;
			token.m_lexeme = consumedString;
			token.m_tokenType = Token::TokenType::SYMBOL;
		}

		return symbol;
	}
}