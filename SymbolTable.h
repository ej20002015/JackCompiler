#pragma once

#include <string>
#include <list>

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
      CLASS,
      ARGUMENT
    };

    enum class SymbolType
    {
      INT,
      BOOLEAN,
      CHAR,
      CLASS,
      VOID
    };

    SymbolKind m_kind;
    SymbolType m_type;
    std::string m_name;
    unsigned m_offset;
  };

  class SymbolTable
  {
  public:
    SymbolTable() : m_tableName("NoName"), m_offset(0) {}
    SymbolTable(std::string& tableName) : m_tableName(tableName), m_offset(0) {}
    void addSymbol(const std::string& name, const Symbol::SymbolKind& symbolKind, const Symbol::SymbolType& symbolType);
    bool checkSymbolExists(const std::string& name) const;
    const std::string& getTableName() const { return m_tableName; }
    const unsigned getOffset() const { return m_offset; }

  private:
    std::list<Symbol> m_symbols;
    std::string m_tableName;
    unsigned m_offset;
  };

  struct SymbolToBeResolved
  {
    std::string m_name;
    unsigned m_lineNum;
  };
}