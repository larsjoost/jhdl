#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier.hpp"
#include "association_element_formal_part.hpp"

namespace vhdl {
  namespace parser {
  
    AssociationElementFormalPart* AssociationElementFormalPart::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      name = scanner->accept<BasicIdentifier>();
      scanner->accept("=>");
      return this;
    }
    
  }
}
