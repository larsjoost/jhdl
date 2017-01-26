#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"
#include "variable_assignment.hpp"

namespace vhdl {
  namespace parser {
  
    VariableAssignment* VariableAssignment::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      BasicIdentifier* i = scanner->expect<BasicIdentifier>();
      identifier = i->text;
      scanner->skipWhiteSpace();
      scanner->expect(":=");
      scanner->skipWhiteSpace();
      expression = scanner->expect<Expression>();
      scanner->skipWhiteSpace();
      scanner->expect(";");
      return this;
    }

  }
}
