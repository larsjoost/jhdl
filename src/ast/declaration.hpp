
#ifndef AST_DECLARATION_H_
#define AST_DECLARATION_H_

#include "text.hpp"
#include "list.hpp"
#include "type_declaration.hpp"
#include "variable_declaration.hpp"
#include "signal_declaration.hpp"
#include "constant_declaration.hpp"
#include "function_declaration.hpp"

namespace ast {
    
  class FunctionDeclaration;

  class Declaration {
    
  public:

    TypeDeclaration* type;
    VariableDeclaration* variable;
    SignalDeclaration* signal;
    ConstantDeclaration* constant;
    FunctionDeclaration* function;
    
  };

}

#endif
