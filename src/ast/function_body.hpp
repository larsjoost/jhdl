
#ifndef AST_FUNCTIONBODY_HPP_
#define AST_FUNCTIONBODY_HPP_

#include "simple_identifier.hpp"
#include "list.hpp"
#include "sequential_statement.hpp"
#include "declaration.hpp"
#include "expression.hpp"

namespace ast {

  class Declaration;
  
  class FunctionBody {
    
  public:
    
    SimpleIdentifier* name;
    SimpleIdentifier* returnType = NULL;
    List<Declaration> declarations;
    List<SequentialStatement> sequentialStatements;
    
  };

}

#endif
