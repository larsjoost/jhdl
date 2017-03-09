#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "range_type.hpp"
#include "subtype_indication.hpp"
#include "basic_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    SubtypeIndication* SubtypeIndication::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      name = scanner->accept<BasicIdentifier>();
      if (scanner->optional(scanner::Scanner::VHDL_RANGE)) {
        range = scanner->accept<RangeType>();
      }
      return this;
    }

  }
}
