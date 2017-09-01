
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "choise.hpp"
#include "range_direction.hpp"
#include "simple_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    Choise* Choise::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      (
       (others = scanner->optional(scanner::Scanner::VHDL_OTHERS)) ||
       (discrete_range = scanner->optional<RangeDirection>()) ||
       (identifier = scanner->optional<SimpleIdentifier>()));
      return this;
    }

  }
}

