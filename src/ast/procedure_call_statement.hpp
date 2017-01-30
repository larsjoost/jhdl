
#ifndef AST_PROCEDURECALLSTATEMENT_H_
#define AST_PROCEDURECALLSTATEMENT_H_

#include "list.hpp"
#include "basic_identifier.hpp"
#include "association_list.hpp"

namespace ast {
    
  class ProcedureCallStatement {
    
  public:

    BasicIdentifier* name;
    AssociationList* associationList;
    
  };

}

#endif
