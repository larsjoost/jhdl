
#ifndef VHDL_PARSER_INTERFACE_ELEMENT_H_
#define VHDL_PARSER_INTERFACE_ELEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/interface_element.hpp"

namespace vhdl {
  namespace parser {

    class InterfaceElement :
      public ::ast::InterfaceElement  {

    public:

      InterfaceElement* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
