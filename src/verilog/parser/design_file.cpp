#include "../scanner/scanner.hpp"
#include "design_file.hpp"
#include "design_unit.hpp"

namespace verilog {
  namespace parser {

    DesignFile* DesignFile::parse(scanner::Scanner* scanner) {
      designUnits.add(scanner->expect<DesignUnit>());
      return this;
    }

  }
}
