
#ifndef VHDL_PARSER_ASSOCIATION_ELEMENT_FORMAL_PART_HPP_
#define VHDL_PARSER_ASSOCIATION_ELEMENT_FORMAL_PART_HPP_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/association_element_formal_part.hpp"

namespace vhdl {
  namespace parser {

    class AssociationElementFormalPart :
      public ::ast::AssociationElementFormalPart  {

    public:

      AssociationElementFormalPart* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
