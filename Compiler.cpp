#include "Compiler.h"

#include <iostream>
#include <dirent.h>

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

		DIR* directory;
		struct dirent* entry;
		directory = opendir(directoryPath.c_str());
		if (directory == NULL)
		{
			COMPILEERROR("No directory exists with the name \"" + directoryPath + "\"")
			return 2;
		}

		for (entry = readdir(directory); entry != NULL; entry = readdir(directory))
		{
			std::string fileString = directoryPath + "/" + entry->d_name;
			if (fileString.substr(fileString.find_last_of(".") + 1) == "jack")
				filePaths.push_back(fileString);
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
