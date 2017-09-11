#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "range_attribute_type.hpp"
#include "simple_identifier.hpp"


namespace vhdl {
  namespace parser {
  
    RangeAttributeType* RangeAttributeType::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      identifier = scanner->accept<SimpleIdentifier>();
      scanner->accept("'");
      scanner->accept(scanner::Scanner::VHDL_RANGE);
      return this;
    }

  }
}
