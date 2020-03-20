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
    SubroutineSymbol(const std::vector<std::string>& parameterList) : Symbol(), m_parameterList(parameterList) {}
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
    
    out << ">, " << symbol->m_offset << ">";
		return out;
	}

  class SymbolTable
  {
  public:
    SymbolTable() : m_tableName("No Name") {}
    SymbolTable(const std::string& tableName) : m_tableName(tableName) {}
    void addSymbol(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const std::string& symbolType);
    void addSymbol(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const std::string& symbolType, const std::vector<std::string>& parameterList);
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
    friend std::ostream& operator << (std::ostream& out, const std::shared_ptr<SymbolTable>& symbolTable);

  private:
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
    bool checkSymbolExistsInAllSymbolTables(const std::string& name, const Symbol::SymbolKind& symbolKind) const;
    //used to check declarations of local variables against local variables and arguments declared in the same scope
    bool checkSymbolExistsInCurrentSymbolTable(const std::string& name, const Symbol::SymbolKind& symbolKind) const;
    bool checkClassDefined(const std::string& className) const;
    void addToSymbolTables(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const std::string& symbolType);
    void addToSymbolTables(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const std::string& symbolType, const std::vector<std::string> parameterList);
    void setSymbolInitialised(const std::string& name);

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
    unsigned m_lineNum;
    Symbol::SymbolKind m_kind;
  };

  inline std::ostream& operator << (std::ostream& out, const SymbolToBeResolved& symbolToBeResolved)
	{
    out << "<" << symbolToBeResolved.m_name << ", " << symbolToBeResolved.m_lineNum << Symbol::m_symbolKindMapping.at(symbolToBeResolved.m_kind) << std::endl;
		return out;
	}
}