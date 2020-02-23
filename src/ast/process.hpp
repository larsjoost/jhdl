
#ifndef AST_PROCESS_H_
#define AST_PROCESS_H_

#include <string>

#include "simple_identifier.hpp"
#include "list.hpp"
#include "sequential_statement.hpp"
#include "declaration.hpp"

namespace ast {
    
  class Process {
    
  public:
    
    std::string noname;
    SimpleIdentifier* label;
    List<SimpleIdentifier>* sensitivity;
    List<Declaration> declarations;
    List<SequentialStatement> sequentialStatements;
    
  };

}

#endif
