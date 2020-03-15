#include "SymbolTable.h"

#include <algorithm>

namespace JackCompiler
{
  void SymbolTable::addSymbol(const std::string& name, const Symbol::SymbolKind& symbolKind, const Symbol::SymbolType& symbolType)
  {
    Symbol newSymbol = {symbolKind, symbolType, name, m_offset++};
    m_symbols.push_back(newSymbol);
  }

  bool SymbolTable::checkSymbolExists(const std::string& name) const
  {
    for (Symbol symbol : m_symbols)
    {
      if (symbol.m_name == name)
        return true;
    }
    return false;
  }
}