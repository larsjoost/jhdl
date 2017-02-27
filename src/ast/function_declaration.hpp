
#ifndef AST_FUNCTIONDECLARATION_H_
#define AST_FUNCTIONDECLARATION_H_

#include "basic_identifier.hpp"
#include "basic_identifier_list.hpp"
#include "list.hpp"
#include "interface_list.hpp"
#include "sequential_statement.hpp"
#include "declaration.hpp"
#include "expression.hpp"

namespace ast {

  class Declaration;
  
  class FunctionDeclaration {
    
  public:
    
    BasicIdentifier* name;
    BasicIdentifier* returnType;
    Expression* returnValue;
    InterfaceList* interface;
    List<Declaration> declarations;
    List<SequentialStatement> sequentialStatements;
    
  };

}

#endif
