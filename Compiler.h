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
    void addLibrarySymbols();
    void writeOutputCodeToConsole(const std::vector<std::string>& outputCode) const;
    void writeOutputCodeToFile(const std::string& filePath, const std::vector<std::string>& outputCode) const;
		std::vector<std::string> m_filePaths;
    SymbolTables m_symbolTables;
    //used to store any symbols that need to be resolved at a later date
    std::list<SymbolToBeResolved> m_symbolsToBeResolved;
	};
}
