
#ifndef AST_INTERFACEELEMENT_H_
#define AST_INTERFACEELEMENT_H_

#include "list.hpp"
#include "variable_declaration.hpp"
#include "signal_declaration.hpp"
#include "constant_declaration.hpp"

namespace ast {
    
  class InterfaceElement {
    
  public:

    VariableDeclaration* variable;
    SignalDeclaration* signal;
    ConstantDeclaration* constant;
    
  };

}

#endif
