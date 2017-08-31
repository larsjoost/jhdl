
#ifndef VHDL_PARSER_ELEMENT_ASSOCIATION_H_
#define VHDL_PARSER_ELEMENT_ASSOCIATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/element_association.hpp"

namespace vhdl {
  namespace parser {

    class ElementAssociation :
      public ::ast::ElementAssociation  {

    public:

      ElementAssociation* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
