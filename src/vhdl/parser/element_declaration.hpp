
#ifndef VHDL_PARSER_ELEMENTDECLARATION_H_
#define VHDL_PARSER_ELEMENTDECLARATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/element_declaration.hpp"

namespace vhdl {
  namespace parser {

    class ElementDeclaration :
      public ::ast::ElementDeclaration {

    public:

      ElementDeclaration* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
