
#ifndef VHDL_PARSER_INTERFACE_ELEMENT_H_
#define VHDL_PARSER_INTERFACE_ELEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "variable_declaration.hpp"
#include "signal_declaration.hpp"
#include "constant_declaration.hpp"
#include "../../ast/interface_element.hpp"

namespace vhdl {
  namespace parser {

    template<scanner::Scanner::Keyword defaultType>
    class InterfaceElement :
      public ::ast::InterfaceElement  {

    public:
      
      InterfaceElement<defaultType>* parse(::ast::Scanner<scanner::Scanner>* scanner) {
        ((variable = scanner->optional<VariableDeclaration<defaultType == scanner::Scanner::VHDL_VARIABLE>>()) || 
         (signal = scanner->optional<SignalDeclaration<defaultType == scanner::Scanner::VHDL_SIGNAL>>()) || 
         (constant = scanner->optional<ConstantDeclaration>()));
        return this;
      }

    };

  }

}
  

#endif
