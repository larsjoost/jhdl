#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "number_type.hpp"
#include "range_type.hpp"
#include "physical_type.hpp"

namespace vhdl {
  namespace parser {
  
    NumberType* NumberType::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_RANGE);
      range = scanner->expect<RangeType>();
      physical = scanner->optional<PhysicalType>();
      return this;
    }

  }
}
