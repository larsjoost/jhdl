#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "report_statement.hpp"
#include "simple_identifier.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    ReportStatement* ReportStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      if (scanner->optional(scanner::Scanner::VHDL_ASSERT)) {
        bool parenthis = scanner->optional("(");
        assert_condition = scanner->expect<Expression>();
        if (parenthis) {scanner->expect(")");}
      }
      scanner->accept(scanner::Scanner::VHDL_REPORT);
      message = scanner->expect<Expression>();
      scanner->expect(scanner::Scanner::VHDL_SEVERITY);
      severity = scanner->expect<SimpleIdentifier>();
      return this;
    }

  }
}
