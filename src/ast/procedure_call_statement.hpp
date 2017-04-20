
#ifndef AST_PROCEDURECALLSTATEMENT_H_
#define AST_PROCEDURECALLSTATEMENT_H_

#include "list.hpp"
#include "simple_identifier.hpp"
#include "association_list.hpp"

namespace ast {

  class SimpleIdentifier;
  
  class ProcedureCallStatement {
    
  public:

    SimpleIdentifier* name;
    AssociationList* arguments = NULL; 
    
  };

}

#endif
