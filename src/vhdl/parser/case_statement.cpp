#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "case_statement.hpp"
#include "simple_identifier.hpp"
#include "case_statement_alternative.hpp"
#include "association_list.hpp"

namespace vhdl {
  namespace parser {
  
    CaseStatement* CaseStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      if (scanner->lookAhead(":", 1)) {
        label = scanner->accept<SimpleIdentifier>();
        scanner->accept(":");
      }
      scanner->accept(scanner::Scanner::VHDL_CASE);
      identifier = scanner->expect<SimpleIdentifier>();
      if (scanner->optional("(")) {
        arguments = scanner->expect<AssociationList>();
        scanner->expect(")");
      }
      scanner->expect(scanner::Scanner::VHDL_IS);
      while (alternatives.add(scanner->optional<CaseStatementAlternative>())) {};
      scanner->expect(scanner::Scanner::VHDL_END);
      scanner->expect(scanner::Scanner::VHDL_CASE);
      SimpleIdentifier* i;
      if (i = scanner->optional<SimpleIdentifier>()) {
        if (!label->equals(i)) {
          scanner->error("Identifier '" + i->toString() +
                         "' does not match case name '" +
                         label->toString() + "'");
        }
      }
      return this;
    }

  }
}
