#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "basic_identifier.hpp"
#include "basic_identifier_list.hpp"
#include "procedure_call_statement.hpp"
#include "association_list.hpp"

namespace vhdl {
  namespace parser {
  
    ProcedureCallStatement* ProcedureCallStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      BasicIdentifier* i = scanner->expect<BasicIdentifier>();
      ::ast::Text name = i->text;
      if (scanner->optional("(")) {
        AssociationList* i = scanner->expect<AssociationList>();
        scanner->expect(")");
      } 
      scanner->skipWhiteSpace();
      scanner->expect(";");
      return this;
    }

  }
}

