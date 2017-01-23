
#ifndef VHDL_PARSER_ASSOCIATION_ELEMENT_H_
#define VHDL_PARSER_ASSOCIATION_ELEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/association_element.hpp"

namespace vhdl {
  namespace parser {

    class AssociationElement :
      public ::ast::AssociationElement  {

    public:

      AssociationElement* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
