




#ifndef AST_PROCESS_H_
#define AST_PROCESS_H_

#include <string>

#include "simple_identifier.hpp"
#include "list.hpp"
#include "sequential_statement.hpp"
#include "declaration.hpp"
#include "text.hpp"

namespace ast {
    
  class Process {
    
  public:

    SimpleIdentifier* label = NULL;
    Text* process = NULL;
    List<SimpleIdentifier>* sensitivity_list = NULL;
    Text* sensitivity_all = NULL;
    List<Declaration> declarations;
    List<SequentialStatement> sequentialStatements;
    
  };

}

#endif
