#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "enumeration_type.hpp"
#include "basic_identifier_list.hpp"

namespace vhdl {
  namespace parser {
  
    EnumerationType* EnumerationType::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      scanner->accept("(");
      enumerations = scanner->expect<BasicIdentifierList<CommaSeparation>>();
      scanner->expect(")");
      return this;
    }

  }
}
