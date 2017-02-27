#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "variable_declaration.hpp"
#include "signal_declaration.hpp"
#include "constant_declaration.hpp"
#include "interface_element.hpp"

namespace vhdl {
  namespace parser {
  
    InterfaceElement* InterfaceElement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      ((variable = scanner->optional<VariableDeclaration>()) || 
       (signal = scanner->optional<SignalDeclaration>()) || 
       (constant = scanner->optional<ConstantDeclaration>()));
      return this;
    }

  }
}

