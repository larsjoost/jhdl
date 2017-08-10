#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "iteration_scheme.hpp"
#include "range_type.hpp"
#include "simple_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    IterationScheme* IterationScheme::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      range = scanner->optional<RangeType>(); 
      if (identifier = scanner->optional<SimpleIdentifier>()) {
        if (scanner->optional("'")) {
          range_attribute = scanner->expect(scanner::Scanner::VHDL_RANGE) ? true : false;
        }
      }
      return this;
    }

  }
}
