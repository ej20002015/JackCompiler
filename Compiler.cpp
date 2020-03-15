#include "Compiler.h"

#include <iostream>
#include <dirent.h>

#include "Core.h"
#include "Lexer.h"
#include "Parser.h"

namespace JackCompiler
{
  Compiler::Compiler()
  {
    //create the global symbol table
    m_symbolTables.push_back(SymbolTable());
  }

	int Compiler::run(int argc, char** argv)
	{
		std::string directoryPath = "";
		if (argc < 2)
			compilerError("No directory name supplied");
		else
			directoryPath = argv[1];

		DIR* directory;
		struct dirent* entry;
		directory = opendir(directoryPath.c_str());
		if (directory == NULL)
			compilerError("No directory exists with the name \"" + directoryPath + "\"");

		for (entry = readdir(directory); entry != NULL; entry = readdir(directory))
		{
			std::string fileString = directoryPath + "/" + entry->d_name;
			if (fileString.substr(fileString.find_last_of(".") + 1) == "jack")
				m_filePaths.push_back(fileString);
		}

		if (m_filePaths.empty())
			compilerError("Directory does not contain any jack files");

		for (std::string filePath : m_filePaths)
			compileFile(filePath);
    
    //if unresolved symbols exist then throw an error
    if (!m_symbolsToBeResolved.empty())
      compilerError("Symbol has not been resolved", m_symbolsToBeResolved.front().m_lineNum, m_symbolsToBeResolved.front().m_name);
		
		//No errors occurred during compilation so return 0
		return 0;
	}

	void Compiler::compileFile(const std::string& filePath)
	{
		std::cout << "Compiling file " << filePath << "..." << std::endl;
		std::cout << std::endl;
		Parser parser(filePath, m_symbolTables, m_symbolsToBeResolved);
		parser.parse();
		std::cout << std::endl;
	}
}