
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "design_unit.hpp"
#include "module.hpp"

namespace verilog {
  namespace parser {

    void DesignUnit::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      m_debug.functionStart("parse");
      module = *(scanner->optional<Module>());
      m_debug.functionEnd("parse");
    }
    
  }
}
