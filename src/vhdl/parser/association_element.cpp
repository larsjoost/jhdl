
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "expression.hpp"
#include "association_element.hpp"
#include "association_element_formal_part.hpp"

namespace vhdl {
  namespace parser {
  
    AssociationElement* AssociationElement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      formalPart = scanner->optional<AssociationElementFormalPart>();
      actualPart = scanner->accept<Expression>();
      return this;
    }

  }
}

