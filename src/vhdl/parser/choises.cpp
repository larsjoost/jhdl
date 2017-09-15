
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "choises.hpp"
#include "choise.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    Choises* Choises::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      do {
        choises.add(scanner->accept<Choise>());
      } while (scanner->optional("|"));
      return this;
    }

  }
}

