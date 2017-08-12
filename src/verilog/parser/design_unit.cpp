
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "design_unit.hpp"
#include "module.hpp"

namespace verilog {
  namespace parser {

    void DesignUnit::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      debug.functionStart("parse");
      module = *(scanner->optional<Module>());
      debug.functionEnd("parse");
    }
    
  }
}
