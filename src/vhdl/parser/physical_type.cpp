#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "physical_type.hpp"
#include "physical_element.hpp"
#include "range_type.hpp"

namespace vhdl {
  namespace parser {
  
    PhysicalType* PhysicalType::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_UNITS);
      while (elements.add(scanner->optional<PhysicalElement>())) {};
      scanner->accept(scanner::Scanner::VHDL_END);
      scanner->accept(scanner::Scanner::VHDL_UNITS);
      return this;
    }

  }
}
