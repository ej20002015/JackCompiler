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
    /**
    * Compiles all the files in the directory entered as a command line argument
    */
		int run(int argc, char** argv);

	private:
    /**
    * Compile the jack file found at the filePath
    */
		void compileFile(const std::string& filePath);
    /**
    * Creates the symbol tables for all the library subroutines 
    */
    void addLibrarySymbols();
    /**
    * Print the array of instructions to the console 
    */
    void writeOutputCodeToConsole(const std::vector<std::string>& outputCode) const;
    /**
    * Write the array of instructions to the text file specified by the filepath
    */
    void writeOutputCodeToFile(const std::string& filePath, const std::vector<std::string>& outputCode) const;
		std::vector<std::string> m_filePaths;
    SymbolTables m_symbolTables;
    //used to store any symbols that need to be resolved at a later date
    std::list<SymbolToBeResolved> m_symbolsToBeResolved;
	};
}
