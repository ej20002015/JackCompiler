#pragma once

#include <vector>
#include <string>

#include "SymbolTable.h"

namespace JackCompiler
{
	class Compiler
	{
	public:
    Compiler();
		int run(int argc, char** argv);

	private:
		void compileFile(const std::string& filePath);
		std::vector<std::string> m_filePaths;
    std::list<SymbolTable> m_symbolTables;
    //used to store any symbols that need to be resolved at a later date
    std::list<SymbolToBeResolved> m_symbolsToBeResolved;
	};
}
