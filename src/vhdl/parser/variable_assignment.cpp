#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"
#include "variable_assignment.hpp"

namespace vhdl {
  namespace parser {
  
    VariableAssignment* VariableAssignment::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      identifier = scanner->accept<BasicIdentifier>();
      scanner->accept(":=");
      expression = scanner->expect<Expression>();
      return this;
    }

  }
}
