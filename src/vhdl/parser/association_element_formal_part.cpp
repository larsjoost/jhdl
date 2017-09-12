#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "simple_identifier.hpp"
#include "association_element_formal_part.hpp"

namespace vhdl {
  namespace parser {
  
    AssociationElementFormalPart* AssociationElementFormalPart::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      if (scanner->optional(scanner::Scanner::VHDL_OTHERS)) {
        others = true;
      } else {
        name = scanner->accept<SimpleIdentifier>();
      }
      scanner->accept("=>");
      return this;
    }
    
  }
}
