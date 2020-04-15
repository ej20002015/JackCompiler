#include "Compiler.h"

#include <iostream>
#include <dirent.h>
#include <algorithm>

#include "Core.h"
#include "Lexer.h"
#include "Parser.h"

namespace JackCompiler
{
  Compiler::Compiler()
  {
    //initialise the static memory segment offset to zero
    SymbolTable::m_offsetStatic = 0;
    addLibrarySymbols();
  }

  void Compiler::addLibrarySymbols()
  {
    //add Math class subroutines
    m_symbolTables.addSymbolTable(SymbolTable("Math"));
    m_symbolTables.addToSymbolTables("Math.abs", Symbol::SymbolKind::FUNCTION, "int", {"int"});
    m_symbolTables.addToSymbolTables("Math.multiply", Symbol::SymbolKind::FUNCTION, "int", {"int", "int"});
    m_symbolTables.addToSymbolTables("Math.divide", Symbol::SymbolKind::FUNCTION, "int", {"int", "int"});
    m_symbolTables.addToSymbolTables("Math.min", Symbol::SymbolKind::FUNCTION, "int", {"int", "int"});
    m_symbolTables.addToSymbolTables("Math.max", Symbol::SymbolKind::FUNCTION, "int", {"int", "int"});
    m_symbolTables.addToSymbolTables("Math.sqrt", Symbol::SymbolKind::FUNCTION, "int", {"int"});

    //add Array class subroutines
    m_symbolTables.addSymbolTable(SymbolTable("Array"));
    m_symbolTables.addToSymbolTables("Array.new", Symbol::SymbolKind::FUNCTION, "Array", {"int"});
    m_symbolTables.addToSymbolTables("Array.dispose", Symbol::SymbolKind::METHOD, "void", {});

    //add Memory class subroutines
    m_symbolTables.addSymbolTable(SymbolTable("Memory"));
    m_symbolTables.addToSymbolTables("Memory.peek", Symbol::SymbolKind::FUNCTION, "int", {"int"});
    m_symbolTables.addToSymbolTables("Memory.poke", Symbol::SymbolKind::FUNCTION, "void", {"int", "int"});
    m_symbolTables.addToSymbolTables("Memory.alloc", Symbol::SymbolKind::FUNCTION, "Array", {"int"});
    m_symbolTables.addToSymbolTables("Memory.deAlloc", Symbol::SymbolKind::FUNCTION, "void", {"any"});

    //add Screen class subroutines
    m_symbolTables.addSymbolTable(SymbolTable("Screen"));
    m_symbolTables.addToSymbolTables("Screen.clearScreen", Symbol::SymbolKind::FUNCTION, "void", {});
    m_symbolTables.addToSymbolTables("Screen.setColor", Symbol::SymbolKind::FUNCTION, "void", {"boolean"});
    m_symbolTables.addToSymbolTables("Screen.drawPixel", Symbol::SymbolKind::FUNCTION, "void", {"int", "int"});
    m_symbolTables.addToSymbolTables("Screen.drawLine", Symbol::SymbolKind::FUNCTION, "void", {"int", "int", "int", "int"});
    m_symbolTables.addToSymbolTables("Screen.drawRectangle", Symbol::SymbolKind::FUNCTION, "void", {"int", "int", "int", "int"});
    m_symbolTables.addToSymbolTables("Screen.drawCircle", Symbol::SymbolKind::FUNCTION, "void", {"int", "int", "int"});

    //add Keyboard class subroutines
    m_symbolTables.addSymbolTable(SymbolTable("Keyboard"));
    m_symbolTables.addToSymbolTables("Keyboard.keyPressed", Symbol::SymbolKind::FUNCTION, "char", {});
    m_symbolTables.addToSymbolTables("Keyboard.readChar", Symbol::SymbolKind::FUNCTION, "char", {});
    m_symbolTables.addToSymbolTables("Keyboard.readLine", Symbol::SymbolKind::FUNCTION, "String", {"String"});
    m_symbolTables.addToSymbolTables("Keyboard.readInt", Symbol::SymbolKind::FUNCTION, "int", {"String"});

    //add Output class subroutines
    m_symbolTables.addSymbolTable(SymbolTable("Output"));
    m_symbolTables.addToSymbolTables("Output.init", Symbol::SymbolKind::FUNCTION, "void", {});
    m_symbolTables.addToSymbolTables("Output.moveCursor", Symbol::SymbolKind::FUNCTION, "void", {"int", "int"});
    m_symbolTables.addToSymbolTables("Output.printChar", Symbol::SymbolKind::FUNCTION, "void", {"char"});
    m_symbolTables.addToSymbolTables("Output.printString", Symbol::SymbolKind::FUNCTION, "void", {"String"});
    m_symbolTables.addToSymbolTables("Output.printInt", Symbol::SymbolKind::FUNCTION, "void", {"int"});
    m_symbolTables.addToSymbolTables("Output.printLn", Symbol::SymbolKind::FUNCTION, "void", {});
    m_symbolTables.addToSymbolTables("Output.backSpace", Symbol::SymbolKind::FUNCTION, "void", {});

    //add String class subroutines
    m_symbolTables.addSymbolTable(SymbolTable("String"));
    m_symbolTables.addToSymbolTables("String.new", Symbol::SymbolKind::CONSTRUCTOR, "String", {"int"});
    m_symbolTables.addToSymbolTables("String.dispose", Symbol::SymbolKind::METHOD, "void", {});
    m_symbolTables.addToSymbolTables("String.length", Symbol::SymbolKind::METHOD, "int", {});
    m_symbolTables.addToSymbolTables("String.charAt", Symbol::SymbolKind::METHOD, "char", {"int"});
    m_symbolTables.addToSymbolTables("String.setCharAt", Symbol::SymbolKind::METHOD, "void", {"int", "char"});
    m_symbolTables.addToSymbolTables("String.appendChar", Symbol::SymbolKind::METHOD, "String", {"char"});
    m_symbolTables.addToSymbolTables("String.eraseLastChar", Symbol::SymbolKind::METHOD, "void", {});
    m_symbolTables.addToSymbolTables("String.intValue", Symbol::SymbolKind::METHOD, "int", {});
    m_symbolTables.addToSymbolTables("String.setInt", Symbol::SymbolKind::METHOD, "void", {"int"});
    m_symbolTables.addToSymbolTables("String.newLine", Symbol::SymbolKind::FUNCTION, "char", {});
    m_symbolTables.addToSymbolTables("String.backSpace", Symbol::SymbolKind::FUNCTION, "char", {});
    m_symbolTables.addToSymbolTables("String.doubleQuote", Symbol::SymbolKind::FUNCTION, "char", {});

    //add Sys class subroutines
    m_symbolTables.addSymbolTable(SymbolTable("Sys"));
    m_symbolTables.addToSymbolTables("Sys.halt", Symbol::SymbolKind::FUNCTION, "void", {});
    m_symbolTables.addToSymbolTables("Sys.error", Symbol::SymbolKind::FUNCTION, "void", {"int"});
    m_symbolTables.addToSymbolTables("Sys.wait", Symbol::SymbolKind::FUNCTION, "void", {"int"});
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
    {
			compileFile(filePath);
    }

    //if unresolved symbols exist then throw an error
    if (!m_symbolsToBeResolved.empty())
      compilerError("Symbol has not been resolved : " + m_symbolsToBeResolved.front().m_fileName, m_symbolsToBeResolved.front().m_lineNum, m_symbolsToBeResolved.front().m_name);
		
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