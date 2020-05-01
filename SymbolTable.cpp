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
      newSymbol.m_initialised = true;
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
      {
        symbol->m_initialised = true;
      }
    }
  }

  bool SymbolTable::checkSymbolInitialised(const std::string& name) const
  {
    for (auto symbol : m_symbols)
    {
      if (symbol->m_name == name && symbol->m_initialised)
        return true;
    }

    return false;
  }

  std::pair<bool, std::string> SymbolTable::getSymbolType(const std::string& name) const
  {
    for (auto symbol : m_symbols)
    {
      if (symbol->m_name == name)
        return std::pair<bool, std::string>{true, symbol->m_type};
    }

    return std::pair<bool, std::string>{false, "NO SUCH SYMBOL"};
  }

  const std::vector<std::string>* SymbolTable::getParameterList(const std::string& subroutineSymbolName) const
  {
    for (auto symbol : m_symbols)
    {
      const std::vector<std::string>* parameterList = symbol->getParameterList();
      if (parameterList && symbol->m_name == subroutineSymbolName)
        return parameterList;
    }

    return nullptr;
  }

  std::pair<int, Symbol::SymbolKind> SymbolTable::getOffsetAndKind(const std::string& symbolName) const
  {
    for (auto symbol : m_symbols)
    {
      if (symbol->m_name == symbolName)
        return std::pair<int , Symbol::SymbolKind>(symbol->m_offset, symbol->m_kind);
    }

    return std::pair<int, Symbol::SymbolKind>(-1, Symbol::SymbolKind::ARGUMENT);
  }

  void SymbolTables::addSymbolTable(const SymbolTable& newSymbolTable)
  {
    //Add a new symbol table to the list of symbol tables
    m_symbolTables.push_back(std::make_shared<SymbolTable>(SymbolTable(newSymbolTable)));
  }

  void SymbolTables::removeCurrentSymbolTable()
  {
    //Remove the current symbol table which is located at the end of the list
    m_symbolTables.pop_back();
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

  void SymbolTables::addToSymbolTables(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const std::string& symbolType)
  {
    m_symbolTables.back()->addSymbol(symbolName, symbolKind, symbolType);
  }

  void SymbolTables::addToSymbolTables(const std::string symbolName, const Symbol::SymbolKind& symbolKind, const std::string& symbolType, const std::vector<std::string> parameterList)
  {
    m_symbolTables.back()->addSymbol(symbolName, symbolKind, symbolType, parameterList);
  }

  void SymbolTables::setSymbolInitialised(const std::string& name)
  {
    for (auto symbolTable : m_symbolTables)
      symbolTable->setSymbolInitialised(name);
  }

  void SymbolTables::setSymbolInitialised(const std::string& name, const std::string& className)
  {
    for (auto symbolTable : m_symbolTables)
      symbolTable->setSymbolInitialised(name);

    if (!checkSymbolInitialised(name))
    { 
      for (auto symbolTable : m_symbolTables)
        symbolTable->setSymbolInitialised(className + "." + name);
    }

  }

  bool SymbolTables::checkSymbolInitialised(const std::string& name) const
  {
    for (auto symbolTable: m_symbolTables)
    {
      if (symbolTable->checkSymbolInitialised(name))
        return true;
    }

    return false;
  }

  bool SymbolTables::checkSymbolInitialised(const std::string& name, const std::string& className) const
  {
    for (auto symbolTable: m_symbolTables)
    {
      if (symbolTable->checkSymbolInitialised(name))
        return true;
    }

    for (auto symbolTable: m_symbolTables)
    {
      if (symbolTable->checkSymbolInitialised(className + "." + name))
        return true;
    }

    return false;
  }

  std::pair<bool, std::string> SymbolTables::getSymbolType(const std::string& name) const
  {
    for (auto symbolTable : m_symbolTables)
    {
      auto symbolTypePair = symbolTable->getSymbolType(name);
      if (symbolTypePair.first == true)
        return symbolTypePair;
    }

    return std::pair<bool, std::string>{false, "NO SUCH SYMBOL"};
  }

  std::pair<bool, std::string> SymbolTables::getSymbolType(const std::string& name, const std::string& className) const
  {
    for (auto symbolTable : m_symbolTables)
    {
      auto symbolTypePair = symbolTable->getSymbolType(name);
      if (symbolTypePair.first == true)
        return symbolTypePair;
    }

    //check for field or static variables

    for (auto symbolTable : m_symbolTables)
    {
      auto symbolTypePair = symbolTable->getSymbolType(className + "." + name);
      if (symbolTypePair.first == true)
        return symbolTypePair;
    }

    return std::pair<bool, std::string>{false, "NO SUCH SYMBOL"};
  }

  const std::vector<std::string>* SymbolTables::getParameterList(const std::string& subroutineSymbolName) const
  {
    for (auto symbolTable : m_symbolTables)
    {
      const std::vector<std::string>* parameterList = symbolTable->getParameterList(subroutineSymbolName);
      if (parameterList)
        return parameterList;
    }

    return nullptr;
  }

  const std::vector<std::string>* SymbolTables::getParameterList(const std::string& subroutineSymbolName, const std::string& className) const
  {
    for (auto symbolTable : m_symbolTables)
    {
      const std::vector<std::string>* parameterList = symbolTable->getParameterList(subroutineSymbolName);
      if (parameterList)
        return parameterList;
    }

    std::string symbolWithClassName = className + "." + subroutineSymbolName;

    for (auto symbolTable : m_symbolTables)
    {
      const std::vector<std::string>* parameterList = symbolTable->getParameterList(symbolWithClassName);
      if (parameterList)
        return parameterList;
    }

    return nullptr;
  }

  std::pair<int, Symbol::SymbolKind> SymbolTables::getOffsetAndKind(const std::string& symbolName) const
  {
    for (auto symbolTable : m_symbolTables)
    {
      auto offsetAndKind = symbolTable->getOffsetAndKind(symbolName);
      if (offsetAndKind.first != -1)
        return offsetAndKind;
    }

    return std::pair<int, Symbol::SymbolKind>(-1, Symbol::SymbolKind::ARGUMENT);
  }

  std::pair<int, Symbol::SymbolKind> SymbolTables::getOffsetAndKind(const std::string& symbolName, const std::string& className) const
  {
    for (auto symbolTable : m_symbolTables)
    {
      auto offsetAndKind = symbolTable->getOffsetAndKind(symbolName);
      if (offsetAndKind.first != -1)
        return offsetAndKind;
    }

    std::string symbolWithClassName = className + "." + symbolName;

    for (auto symbolTable : m_symbolTables)
    {
      auto offsetAndKind = symbolTable->getOffsetAndKind(symbolWithClassName);
      if (offsetAndKind.first != -1)
        return offsetAndKind;
    }

    return std::pair<int, Symbol::SymbolKind>(-1, Symbol::SymbolKind::ARGUMENT);
  }
}