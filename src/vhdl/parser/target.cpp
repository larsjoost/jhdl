#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "target.hpp"
#include "target_argument.hpp"
#include "expression.hpp"
#include "range_type.hpp"
#include "simple_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    Target* Target::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      identifier = scanner->accept<SimpleIdentifier>();
      if (scanner->optional(".")) {
        element = scanner->expect<Target, Target>();
      } else if (scanner->optional("(")) {
        do {
          arguments.add(scanner->accept<TargetArgument>());
        } while (scanner->optional(","));
        scanner->accept(")");
      }
      return this;
    }

  }
}
