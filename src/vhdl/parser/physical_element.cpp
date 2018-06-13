#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "physical_element.hpp"
#include "simple_identifier.hpp"
#include "physical.hpp"

namespace vhdl {
  namespace parser {
  
    PhysicalElement* PhysicalElement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      unit = scanner->accept<SimpleIdentifier>();
      if (scanner->optional("=")) {
        physical = scanner->expect<Physical, PhysicalElement>();
      }
      scanner->expect(";");
      return this;
    }

  }
}
