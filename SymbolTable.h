#pragma once

#include <string>
#include <list>
#include <vector>
#include <map>
#include <iostream>

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
      ARGUMENT
    };

    std::map<SymbolKind, std::string> m_symbolKindMapping;

    enum class SymbolType
    {
      INT,
      BOOLEAN,
      CHAR,
      CLASS,
      VOID,
      NONE
    };

    std::map<SymbolType, std::string> m_symbolTypeMapping;

    Symbol() : m_parameterList(std::vector<SymbolType>({SymbolType::NONE})), m_initialised(false) { setUpMaps(); }
    Symbol(const std::vector<SymbolType>& parameterList) : m_parameterList(std::vector<SymbolType>(parameterList)), m_initialised(false) { setUpMaps(); }
    void setUpMaps();
    bool m_initialised;

    const std::vector<SymbolType> getParameterList() const { return m_parameterList; }

    SymbolKind m_kind;
    SymbolType m_type;
    std::string m_name;
    unsigned m_offset;
    std::vector<SymbolType> m_parameterList;
  };

  inline std::ostream& operator << (std::ostream& out, Symbol const& symbol) 
	{
		out << "<" << symbol.m_name << ", " << symbol.m_symbolKindMapping.at(symbol.m_kind) << ", " << symbol.m_symbolTypeMapping.at(symbol.m_type) << ", ";
    for (Symbol::SymbolType symbolType : symbol.getParameterList())
      out << symbol.m_symbolTypeMapping.at(symbolType);
    out << ", " << symbol.m_offset << ">";
		return out;
	}

  class SymbolTable
  {
  public:
    SymbolTable() : m_tableName("No Name") {}
    SymbolTable(const std::string& tableName) : m_tableName(tableName) {}
    void addSymbol(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const Symbol::SymbolType& symbolType);
    void addSymbol(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const Symbol::SymbolType& symbolType, const std::vector<Symbol::SymbolType> parameterList);
    bool checkSymbolExists(const std::string& name, const Symbol::SymbolKind& symbolKind) const;
    void setSymbolInitialised(const std::string& name);
    const std::string getTableName() const { return m_tableName; }

    static const unsigned m_numOfDifferentOffsets = 3;
    //used as array indexes - do not change
    enum class OffsetsIndex
    {
      ARGUMENT,
      LOCAL,
      FIELD
    };

    static unsigned m_offsetStatic;
    friend std::ostream& operator << (std::ostream& out, SymbolTable const& symbolTable);

  private:
    unsigned m_offsets[m_numOfDifferentOffsets] = {0};
    std::list<Symbol> m_symbols;
    std::string m_tableName;
  };

  inline std::ostream& operator << (std::ostream& out, SymbolTable const& symbolTable)
	{
		out << symbolTable.getTableName() << std::endl;
    for (Symbol symbol : symbolTable.m_symbols)
      out << symbol << std::endl;
    out << std::endl;
		return out;
	}

  class SymbolTables
  {
  public:
    void addSymbolTable(const SymbolTable& newSymbolTable);
    bool checkSymbolExistsInAllSymbolTables(const std::string& name, const Symbol::SymbolKind& symbolKind) const;
    bool checkClassDefined(const std::string& className) const;
    void addToSymbolTables(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const Symbol::SymbolType& symbolType);
    void addToSymbolTables(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const Symbol::SymbolType& symbolType, const std::vector<Symbol::SymbolType> parameterList);
    void setSymbolInitialised(const std::string& name);

    friend std::ostream& operator << (std::ostream& out, SymbolTables const& symbolTables);

  //private:
    std::list<SymbolTable> m_symbolTables;
  };

  inline std::ostream& operator << (std::ostream& out, SymbolTables const& symbolTables)
	{
    for (SymbolTable symbolTable : symbolTables.m_symbolTables)
      out << symbolTable << std::endl;
		return out;
	}

  struct SymbolToBeResolved
  {
    std::string m_name;
    unsigned m_lineNum;
  };

  inline std::ostream& operator << (std::ostream& out, SymbolToBeResolved const& symbolToBeResolved)
	{
    out << "<" << symbolToBeResolved.m_name << ", " << symbolToBeResolved.m_lineNum << std::endl;
		return out;
	}
}