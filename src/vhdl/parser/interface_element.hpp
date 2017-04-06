
#ifndef VHDL_PARSER_INTERFACE_ELEMENT_H_
#define VHDL_PARSER_INTERFACE_ELEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "object_declaration.hpp"
#include "../../ast/object_type.hpp"
#include "../../ast/interface_element.hpp"

namespace vhdl {
  namespace parser {

    template<scanner::Scanner::Keyword defaultType>
    class InterfaceElement :
      public ::ast::InterfaceElement  {

    public:
      
      InterfaceElement<defaultType>* parse(::ast::Scanner<scanner::Scanner>* scanner) {
        if (object = scanner->optional<ObjectDeclaration<scanner::Scanner::VHDL_VARIABLE, defaultType == scanner::Scanner::VHDL_VARIABLE>>()) {
          object->objectType = ast::VARIABLE;
        } else if (object = scanner->optional<ObjectDeclaration<scanner::Scanner::VHDL_SIGNAL, defaultType == scanner::Scanner::VHDL_SIGNAL>>()) {
          object->objectType = ast::SIGNAL;
        } else if (object = scanner->optional<ObjectDeclaration<scanner::Scanner::VHDL_CONSTANT, defaultType == scanner::Scanner::VHDL_CONSTANT>>()) {
          object->objectType = ast::CONSTANT;
        }
        return this;
      }

    };

  }

}
  

#endif
