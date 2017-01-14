
#include "design_unit.hpp"
#include "module.hpp"
#include "../scanner/scanner.hpp"

namespace verilog {
  namespace parser {

    void DesignUnit::parse(scanner::Scanner* scanner) {
       architecture = scanner->optional<Module>();
    }
    
  }
}
