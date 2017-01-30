#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"
#include "variable_assignment.hpp"

namespace vhdl {
  namespace parser {
  
    VariableAssignment* VariableAssignment::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      identifier = scanner->accept<BasicIdentifier>();
      scanner->skipWhiteSpace();
      scanner->accept(":=");
      scanner->skipWhiteSpace();
      expression = scanner->expect<Expression>();
      scanner->skipWhiteSpace();
      scanner->expect(";");
      return this;
    }

  }
}
