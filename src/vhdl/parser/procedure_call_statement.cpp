#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "simple_identifier.hpp"
#include "procedure_call_statement.hpp"

namespace vhdl {
  namespace parser {
  
    ProcedureCallStatement* ProcedureCallStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      name = scanner->accept<SimpleIdentifier>();
      return this;
    }

  }
}

