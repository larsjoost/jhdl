#include "../scanner/scanner.hpp"
#include "design_file.hpp"
#include "design_unit.hpp"

namespace vhdl {
  namespace parser {

    void DesignFile::parse(scanner::Scanner* scanner) {
      designUnits.add(scanner->accept<DesignUnit>());
    }

  }
}
