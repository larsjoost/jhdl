
#ifndef AST_METHOD_H_
#define AST_METHOD_H_

#include <string>

#include "simple_identifier.hpp"
#include "list.hpp"
#include "sequential_statement.hpp"
#include "declaration.hpp"

namespace ast {
    
  class Method {
    
  public:
    
    std::string noname;
    SimpleIdentifier* label;
    List<SimpleIdentifier>* sensitivity;
    List<Declaration> declarations;
    List<SequentialStatement> sequentialStatements;
    
  };

}

#endif
