
#ifndef VHDL_PARSER_ELEMENTDECLARATIONLIST_H_
#define VHDL_PARSER_ELEMENTDECLARATIONLIST_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/element_declaration_list.hpp"

namespace vhdl {
  namespace parser {

    class ElementDeclarationList :
      public ::ast::ElementDeclarationList  {

    public:

      ElementDeclarationList* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
