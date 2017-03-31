#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "enumeration_type.hpp"
#include "enumeration_element.hpp"
#include "list.hpp"

namespace vhdl {
  namespace parser {
  
    EnumerationType* EnumerationType::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept("(");
      enumerations = scanner->expect<List<EnumerationElement, ::ast::EnumerationElement, ','>>();
      scanner->expect(")");
      return this;
    }

  }
}
