#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "enumeration_type.hpp"
#include "simple_identifier.hpp"
#include "list.hpp"

namespace vhdl {
  namespace parser {
  
    EnumerationType* EnumerationType::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept("(");
      enumerations = scanner->expect<List<SimpleIdentifier, ::ast::SimpleIdentifier, ','>>();
      scanner->expect(")");
      return this;
    }

  }
}
