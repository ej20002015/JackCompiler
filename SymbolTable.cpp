#include "SymbolTable.h"

#include <algorithm>

namespace JackCompiler
{
  std::map<Symbol::SymbolKind, std::string> Symbol::m_symbolKindMapping =
  {
    {Symbol::SymbolKind::ARGUMENT, "ARGUMENT"},
    {Symbol::SymbolKind::CONSTRUCTOR, "CONSTRUCTOR"},
    {Symbol::SymbolKind::FIELD, "FIELD"},
    {Symbol::SymbolKind::FUNCTION, "FUNCTION"},
    {Symbol::SymbolKind::METHOD, "METHOD"},
    {Symbol::SymbolKind::STATIC, "STATIC"},
    {Symbol::SymbolKind::VAR, "VAR"},
    {Symbol::SymbolKind::CLASS, "CLASS"}
  };

  unsigned SymbolTable::m_offsetStatic = 0;

  void SymbolTable::addSymbol(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const std::string& symbolType)
  {
    Symbol newSymbol;
    newSymbol.m_kind = symbolKind;
    newSymbol.m_type = symbolType;
    newSymbol.m_name = symbolName;

    switch (symbolKind)
    {
    case Symbol::SymbolKind::ARGUMENT:
      newSymbol.m_offset = m_offsets[(int)OffsetsIndex::ARGUMENT]++;
      break;
    
    case Symbol::SymbolKind::VAR:
      newSymbol.m_offset = m_offsets[(int)OffsetsIndex::LOCAL]++;
      break;

    case Symbol::SymbolKind::FIELD:
      newSymbol.m_offset = m_offsets[(int)OffsetsIndex::FIELD]++;
      break;

    case Symbol::SymbolKind::STATIC:
      newSymbol.m_offset = m_offsetStatic++;
      break;

    default:
      newSymbol.m_offset = -1;
      break;
    }

    m_symbols.push_back(std::make_shared<Symbol>(Symbol(newSymbol)));
  }

  void SymbolTable::addSymbol(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const std::string& symbolType, const std::vector<std::string>& parameterList)
  {
    SubroutineSymbol newSymbol(parameterList);
    newSymbol.m_kind = symbolKind;
    newSymbol.m_type = symbolType;
    newSymbol.m_name = symbolName;
    newSymbol.m_offset = -1; //offset set to -1 as this symbol is a function not a variable
    m_symbols.push_back(std::make_shared<SubroutineSymbol>(SubroutineSymbol(newSymbol)));
  }

  bool SymbolTable::checkSymbolExists(const std::string& name, const Symbol::SymbolKind& symbolKind) const
  { 
    //compare to arguments and local variables if symbol is an argument or local variable
    if (symbolKind == Symbol::SymbolKind::ARGUMENT || symbolKind == Symbol::SymbolKind::VAR)
    {
      for (auto symbol : m_symbols)
      {
        //std::cout << m_symbols.back()->m_symbolTypeMapping.at(symbol->getParameterList()->at(0)) << std::endl;
        if (symbol->m_name == name && (symbol->m_kind == Symbol::SymbolKind::ARGUMENT || symbol->m_kind == Symbol::SymbolKind::VAR))
          return true;
      }
      return false;
    }
    
    //compare to fields and static variables if symbol is a field or static variable
    if (symbolKind == Symbol::SymbolKind::FIELD || symbolKind == Symbol::SymbolKind::STATIC)
    {
      for (auto symbol : m_symbols)
      {
        if (symbol->m_name == name && (symbol->m_kind == Symbol::SymbolKind::FIELD || symbol->m_kind == Symbol::SymbolKind::STATIC))
          return true;
      }
      return false;
    }

    //compare to functions, methods and constructors
    for (auto symbol : m_symbols)
      {
        if (symbol->m_name == name && (symbol->m_kind == Symbol::SymbolKind::FUNCTION || symbol->m_kind == Symbol::SymbolKind::METHOD || symbol->m_kind == Symbol::SymbolKind::CONSTRUCTOR))
          return true;
      }
      return false;
  }

  void SymbolTable::setSymbolInitialised(const std::string& name)
  {
    for (auto symbol : m_symbols)
    {
      if (symbol->m_name == name)
        symbol->m_initialised = true;
    }
  }

  void SymbolTables::addSymbolTable(const SymbolTable& newSymbolTable)
  {
    m_symbolTables.push_back(std::make_shared<SymbolTable>(SymbolTable(newSymbolTable)));
  }

  bool SymbolTables::checkSymbolExistsInAllSymbolTables(const std::string& name, const Symbol::SymbolKind& symbolKind) const
  {
    for (auto symbolTable : m_symbolTables)
    {
      if (symbolTable->checkSymbolExists(name, symbolKind))
        return true;
    }

    return false;
  }

  bool SymbolTables::checkSymbolExistsInCurrentSymbolTable(const std::string& name, const Symbol::SymbolKind& symbolKind) const
  {
    auto currentSymbolTable = m_symbolTables.back();
    return currentSymbolTable->checkSymbolExists(name, symbolKind);
  }

  bool SymbolTables::checkClassDefined(const std::string& className) const
  {
    for (auto symbolTable : m_symbolTables)
    {
      if (symbolTable->getTableName() == className)
        return true;
    }

    return false;
  }

  void SymbolTables::setSymbolInitialised(const std::string& name)
  {
    for (auto symbolTable : m_symbolTables)
    {
      symbolTable->setSymbolInitialised(name);
    }
  }

  void SymbolTables::addToSymbolTables(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const std::string& symbolType)
  {
    m_symbolTables.back()->addSymbol(symbolName, symbolKind, symbolType);
  }

  void SymbolTables::addToSymbolTables(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const std::string& symbolType, const std::vector<std::string> parameterList)
  {
    m_symbolTables.back()->addSymbol(symbolName, symbolKind, symbolType, parameterList);
  }
}