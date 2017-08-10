#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "simple_identifier.hpp"
#include "expression.hpp"
#include "variable_assignment.hpp"
#include "association_list.hpp"

namespace vhdl {
  namespace parser {
  
    VariableAssignment* VariableAssignment::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      identifier = scanner->accept<SimpleIdentifier>();
      if (scanner->optional("(")) {
        arguments = scanner->expect<AssociationList>();
        scanner->expect(")");
      }
      scanner->accept(":=");
      expression = scanner->expect<Expression>();
      return this;
    }

  }
}
