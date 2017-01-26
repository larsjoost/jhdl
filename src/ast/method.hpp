
#ifndef AST_METHOD_H_
#define AST_METHOD_H_

#include "text.hpp"
#include "list.hpp"
#include "sequential_statement.hpp"
#include "declaration.hpp"

namespace ast {
    
  class Method {
    
  public:
    
    Text* name;
    List<Declaration> declarations;
    List<SequentialStatement> sequentialStatements;
    
  };

}

#endif
