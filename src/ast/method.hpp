
#ifndef AST_METHOD_H_
#define AST_METHOD_H_

#include "basic_identifier.hpp"
#include "list.hpp"
#include "sequential_statement.hpp"
#include "declaration.hpp"

namespace ast {
    
  class Method {
    
  public:
    
    BasicIdentifier* name;
    List<Declaration> declarations;
    List<SequentialStatement> sequentialStatements;
    
  };

}

#endif
