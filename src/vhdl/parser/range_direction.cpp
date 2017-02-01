#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "range_direction.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    RangeDirection* RangeDirection::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      if (scanner->optional(scanner::Scanner::VHDL_TO)) {
	direction = TO;
      } else if (scanner->optional(scanner::Scanner::VHDL_DOWNTO)) {
	direction = DOWNTO;
      }
      return this;
    }

  }
}
