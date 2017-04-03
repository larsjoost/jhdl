
#ifndef AST_PROCEDUREBODY_HPP_
#define AST_PROCEDUREBODY_HPP_

#include "simple_identifier.hpp"
#include "list.hpp"
#include "sequential_statement.hpp"
#include "declaration.hpp"
#include "expression.hpp"

namespace ast {

  class Declaration;
  
  class ProcedureBody {
    
  public:
    
    SimpleIdentifier* name;
    List<Declaration> declarations;
    List<SequentialStatement> sequentialStatements;
    
  };

}

#endif
