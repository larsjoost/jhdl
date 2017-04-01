#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "array_subtype_definition.hpp"
#include "simple_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    ArraySubtypeDefinition* ArraySubtypeDefinition::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      identifier = scanner->accept<SimpleIdentifier>();
      scanner->accept(scanner::Scanner::VHDL_RANGE);
      scanner->expect("<");
      scanner->expect(">");
      return this;
    }

  }
}
