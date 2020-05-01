#pragma once

#include <string>
#include <list>
#include <vector>
#include <map>
#include <iostream>
#include <memory>

namespace JackCompiler
{
  struct Symbol
  {
    enum class SymbolKind
    {
      STATIC,
      FIELD,
      VAR,
      FUNCTION,
      METHOD,
      CONSTRUCTOR,
      ARGUMENT,
      CLASS
    };

    static std::map<SymbolKind, std::string> m_symbolKindMapping;

    Symbol() :  m_initialised(false) {}
    virtual const std::vector<std::string>* const getParameterList() const { return nullptr; }

    SymbolKind m_kind;
    std::string m_type;
    std::string m_name;
    int m_offset;
    bool m_initialised;
  };

  class SubroutineSymbol : public Symbol
  {
  public:
    SubroutineSymbol(const std::vector<std::string>& parameterList) : Symbol(), m_parameterList(parameterList) { m_initialised = true; }
    const std::vector<std::string>* const getParameterList() const override { return &m_parameterList; }

  private:
    std::vector<std::string> m_parameterList;
  };


  inline std::ostream& operator << (std::ostream& out, const std::shared_ptr<Symbol>& symbol) 
	{
		out << "<" << symbol->m_name << ", " << Symbol::m_symbolKindMapping.at(symbol->m_kind) << ", " << symbol->m_type << ", <";
    if (symbol->getParameterList())
    {
      for (unsigned i = 0; i < symbol->getParameterList()->size(); ++i)
      {
        out << symbol->getParameterList()->at(i);
        if (i < symbol->getParameterList()->size() - 1)
          out << ", ";
      }
    }
    else
      out << "NO PARAMETER LIST";
    
    out << ">, " << symbol->m_offset << ", " << symbol->m_initialised << ">";
		return out;
	}

  class SymbolTable
  {
  public:
    /**
    * Create a symbol table with no name - used for subroutines
    */
    SymbolTable() : m_tableName("No Name") {}
    /**
    * Create a symbol table with a name - used for classes
    */ 
    SymbolTable(const std::string& tableName) : m_tableName(tableName) {}
    /**
    * Add a symbol to the table
    */
    void addSymbol(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const std::string& symbolType);
    /**
    * Add a subroutine symbol to the table
    */
    void addSymbol(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const std::string& symbolType, const std::vector<std::string>& parameterList);
    /**
    * Check if a certain symbol exists in the symbol table
    */
    bool checkSymbolExists(const std::string& name, const Symbol::SymbolKind& symbolKind) const;
    /**
    * Set a symbol as initialised
    */
    void setSymbolInitialised(const std::string& name);
    /**
    * Returns a boolean indicating whether the given symbol is initialised
    */
    bool checkSymbolInitialised(const std::string& name) const;
    std::list<std::shared_ptr<Symbol>> getSymbols() const { return m_symbols; };
    const std::string getTableName() const { return m_tableName; }
    /**
    * Given the name of a symbol return the type of the symbol along with a boolean indicating whether the symbol was found
    */
    std::pair<bool, std::string> getSymbolType(const std::string& name) const;
    /**
    * Return the parameter list of a subroutine symbol
    */
    const std::vector<std::string>* getParameterList(const std::string& subroutineSymbolName) const;
    /**
    * Get the offset and kind of a symbol with symbolName
    */
    std::pair<int, Symbol::SymbolKind> getOffsetAndKind(const std::string& symbolName) const;
    /**
    * Get the offset and kind of a symbol with symbolName or className.symbolName
    */
    std::pair<int, Symbol::SymbolKind> getOffsetAndKind(const std::string& symbolName, const std::string& className) const;
    

    static const unsigned m_numOfDifferentOffsets = 3;
    //used as array indexes - do not change
    enum class OffsetsIndex
    {
      ARGUMENT,
      LOCAL,
      FIELD
    };

    static unsigned m_offsetStatic;
    friend std::ostream& operator << (std::ostream& out, const std::shared_ptr<SymbolTable>& symbolTable);

  private:
    //Initialise all the offsets to 0
    unsigned m_offsets[m_numOfDifferentOffsets] = {0};
    std::list<std::shared_ptr<Symbol>> m_symbols;
    std::string m_tableName;
  };

  inline std::ostream& operator << (std::ostream& out, const std::shared_ptr<SymbolTable>& symbolTable)
	{
		out << symbolTable->getTableName() << std::endl;
    for (auto symbol : symbolTable->m_symbols)
      out << symbol << std::endl;

    out << std::endl;
		return out;
	}

  class SymbolTables
  {
  public:
    void addSymbolTable(const SymbolTable& newSymbolTable);
    /**
    * Remove the current symbol table at the end of the list
    */
    void removeCurrentSymbolTable();
    /**
    * Indicates if a symbol exists in all the symbol tables - used for calls to subroutines and field/static variables
    */
    bool checkSymbolExistsInAllSymbolTables(const std::string& name, const Symbol::SymbolKind& symbolKind) const;
    /**
    * Indicates if a symbol exists in the current symbol table at the end of the list - used to check declarations of local variables against local variables and arguments declared in the same scope
    */
    bool checkSymbolExistsInCurrentSymbolTable(const std::string& name, const Symbol::SymbolKind& symbolKind) const;
    /**
    * Returns a boolean indicating whether the className given is the same as a class that has already been declared
    */
    bool checkClassDefined(const std::string& className) const;
    /**
    * Add a symbol to the current symbol table
    */
    void addToSymbolTables(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const std::string& symbolType);
    /**
    * Add a function symbol to the current symbol table
    */
    void addToSymbolTables(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const std::string& symbolType, const std::vector<std::string> parameterList);
    /**
    * Set the symbol given to initialised in the most localised scope (from subroutine symbol table back to class symbol tables) where the symbol name is just symbolName
    */
    void setSymbolInitialised(const std::string& name);
    /**
    * Set the symbol given to initialised in the most localised scope (from subroutine symbol table back to class symbol tables) where the symbol name is symbolName and then className.symbolName
    */
    void setSymbolInitialised(const std::string& name, const std::string& className);
    /**
    * Return a boolean indicating whether the symbol given is initialised in the most localised scope (from subroutine symbol table back to class symbol tables) where the symbol name is just symbolName
    */
    bool checkSymbolInitialised(const std::string& name) const;
    /**
    * Return a boolean indicating whether the symbol given is initialised in the most localised scope (from subroutine symbol table back to class symbol tables) where the symbol name is symbolName and then className.symbolName
    */
    bool checkSymbolInitialised(const std::string& name, const std::string& className) const;
    /**
    * Return the symbols from the current symbol table at the end of the list
    */
    std::list<std::shared_ptr<Symbol>> getSymbolsFromCurrentSymbolTable() const { return m_symbolTables.back()->getSymbols(); };
    /**
    * Return the data type of the symbol given in the most localised scope (from subroutine symbol table back to class symbol tables) along with a boolean indicating whether the symbol was found. The symbol name is just symbolName.
    */
    std::pair<bool, std::string> getSymbolType(const std::string& name) const;
    /**
    * Return the data type of the symbol given in the most localised scope (from subroutine symbol table back to class symbol tables) along with a boolean indicating whether the symbol was found. The symbol name is symbolName and then className.symbolName.
    */
    std::pair<bool, std::string> getSymbolType(const std::string& name, const std::string& className) const;
    const std::list<std::shared_ptr<SymbolTable>>& getSymbolTables() const { return m_symbolTables; }
    /**
    * Return the parameterList of the given symbol in the most localised scope (from subroutine symbol table back to class symbol tables). The symbol name is only symbolName.
    */
    const std::vector<std::string>* getParameterList(const std::string& subroutineSymbolName) const;
    /**
    * Return the parameterList of the given symbol in the most localised scope (from subroutine symbol table back to class symbol tables). The symbol name is symbolName and then className.symbolName.
    */
    const std::vector<std::string>* getParameterList(const std::string& subroutineSymbolName, const std::string& className) const;
    /**
    * Return the offset (-1 if not found) of the given symbol along with its kind. Get a matching symbol from the most localised scope (from subroutine symbol table back to class symbol tables). The symbol name is only symbolName.
    */
    std::pair<int, Symbol::SymbolKind> getOffsetAndKind(const std::string& symbolName) const;
    /**
    * Return the offset (-1 if not found) of the given symbol along with its kind. Get a matching symbol from the most localised scope (from subroutine symbol table back to class symbol tables). The symbol name is symbolName and then className.symbolName.
    */
    std::pair<int, Symbol::SymbolKind> getOffsetAndKind(const std::string& symbolName, const std::string& className) const;

    friend std::ostream& operator << (std::ostream& out, const SymbolTables& symbolTables);

  private:
    std::list<std::shared_ptr<SymbolTable>> m_symbolTables;
  };

  inline std::ostream& operator << (std::ostream& out, const SymbolTables& symbolTables)
	{
    for (auto symbolTable : symbolTables.m_symbolTables)
      out << symbolTable << std::endl;
		return out;
	}

  struct SymbolToBeResolved
  {
    std::string m_name;
    std::string m_fileName;
    unsigned m_lineNum;
    Symbol::SymbolKind m_kind;
    //the bool indicates whether the vector should be compared against
    std::pair<bool, std::vector<std::string>> m_parameterList;

    SymbolToBeResolved() : m_name(""), m_fileName(""), m_lineNum(0), m_kind(Symbol::SymbolKind::ARGUMENT), m_parameterList(std::pair<bool, std::vector<std::string>>(false, std::vector<std::string>())) {}
    SymbolToBeResolved(const std::string& name, const std::string& fileName, unsigned lineNum, Symbol::SymbolKind kind, std::pair<bool, std::vector<std::string>> parameterList) : m_name(name), m_fileName(fileName), m_lineNum(lineNum), m_kind(kind), m_parameterList(parameterList) {}
  };

  inline std::ostream& operator << (std::ostream& out, const SymbolToBeResolved& symbolToBeResolved)
	{
    out << "<" << symbolToBeResolved.m_name << ", " << symbolToBeResolved.m_fileName << ", " << symbolToBeResolved.m_lineNum << Symbol::m_symbolKindMapping.at(symbolToBeResolved.m_kind) << ", <";
    if (symbolToBeResolved.m_parameterList.first)
      for (int i = 0; i < symbolToBeResolved.m_parameterList.second.size(); ++i)
      {
        out << symbolToBeResolved.m_parameterList.second.at(i);
        if (i < symbolToBeResolved.m_parameterList.second.size() - 1)
          out << ", ";
      }
      return out << ">>" << std::endl;
	}
}