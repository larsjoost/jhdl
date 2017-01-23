#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "design_file.hpp"
#include "design_unit.hpp"

namespace vhdl {
  namespace parser {

    DesignFile* DesignFile::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      designUnits.add(scanner->expect<DesignUnit>());
      return this;
    }

  }
}
