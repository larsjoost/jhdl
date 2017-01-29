#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier.hpp"
#include "basic_identifier_list.hpp"
#include "procedure_call_statement.hpp"
#include "association_list.hpp"

namespace vhdl {
  namespace parser {
  
    ProcedureCallStatement* ProcedureCallStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      BasicIdentifier* i = scanner->accept<BasicIdentifier>();
      name = i->text;
      if (scanner->optional("(")) {
        associationList = scanner->expect<AssociationList>();
        scanner->expect(")");
      } 
      scanner->skipWhiteSpace();
      scanner->expect(";");
      return this;
    }

  }
}

