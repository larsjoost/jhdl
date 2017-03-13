
#ifndef AST_PROCEDURECALLSTATEMENT_H_
#define AST_PROCEDURECALLSTATEMENT_H_

#include "list.hpp"
#include "basic_identifier.hpp"

namespace ast {

  class BasicIdentifier;
  
  class ProcedureCallStatement {
    
  public:

    BasicIdentifier* name;
    
  };

}

#endif
