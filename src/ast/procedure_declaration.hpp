
#ifndef AST_PROCEDUREDECLARATION_H_
#define AST_PROCEDUREDECLARATION_H_

#include "simple_identifier.hpp"
#include "interface_list.hpp"
#include "procedure_body.hpp"

namespace ast {

  class ProcedureBody;
  
  class ProcedureDeclaration {
    
  public:
    
    SimpleIdentifier* name;
    SimpleIdentifier* returnType;
    InterfaceList* interface;
    ProcedureBody* body;
    
  };

}

#endif
