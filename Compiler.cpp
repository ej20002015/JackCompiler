#include "Compiler.h"

#include <iostream>
#include <filesystem>

#include "Core.h"
#include "Lexer.h"

namespace JackCompiler
{
	int Compiler::run(int argc, char** argv)
	{
		std::string directoryPath = "";
		if (argc < 2)
		{
			COMPILEERROR("No directory name supplied")
			return 1;
		}
		else
			directoryPath = argv[1];

		if (!std::filesystem::is_directory(directoryPath))
		{
			COMPILEERROR("No directory exists with the name \"" + directoryPath + "\"")
			return 2;
		}

		try
		{
			for (const auto& file : std::filesystem::directory_iterator(directoryPath))
			{
				std::string fileString = file.path().string();
				if (fileString.substr(fileString.find_last_of(".") + 1) == "jack")
					filePaths.push_back(fileString);
			}
		}
		catch (std::filesystem::filesystem_error& error)
		{
			COMPILEERROR("Error with filesystem -> " + std::string(error.what()))
			return 3;
		}

		if (filePaths.empty())
		{
			COMPILEERROR("Directory does not contain any jack files")
			return 4;
		}

		for (std::string filePath : filePaths)
			compileFile(filePath);
		
		//No errors occured during compilation so return 0
		return 0;
	}

	void Compiler::compileFile(std::string& filePath)
	{
		std::cout << "Compiling file " << filePath << "..." << std::endl;
		std::cout << std::endl;
		Lexer lexer(filePath);
		Token token;
		while (token.m_tokenType != Token::TokenType::EOFILE)
		{
			token = lexer.getNextToken();
			std::cout << token << std::endl;
		}
		std::cout << std::endl;
	}
}
