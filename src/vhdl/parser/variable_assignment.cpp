#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"
#include "variable_assignment.hpp"
#include "association_list.hpp"

namespace vhdl {
  namespace parser {
  
    VariableAssignment* VariableAssignment::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      target = scanner->accept<BasicIdentifier>();
      scanner->accept(":=");
      new_operation = scanner->optional(scanner::Scanner::VHDL_NEW) ? true : false;
      expression = scanner->expect<Expression>();
      return this;
    }

  }
}
