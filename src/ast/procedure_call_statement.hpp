
#ifndef AST_PROCEDURECALLSTATEMENT_H_
#define AST_PROCEDURECALLSTATEMENT_H_

#include "list.hpp"
#include "text.hpp"
#include "association_list.hpp"

namespace ast {
    
  class ProcedureCallStatement {
    
  public:

    Text name;
    AssociationList* associationList;
    
  };

}

#endif
