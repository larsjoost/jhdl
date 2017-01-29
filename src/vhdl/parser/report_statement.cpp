#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "report_statement.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    ReportStatement* ReportStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      scanner->accept(scanner::Scanner::VHDL_REPORT);
      scanner->skipWhiteSpace();
      message = scanner->expect<Expression>();
      scanner->skipWhiteSpace();
      scanner->expect(scanner::Scanner::VHDL_SEVERITY);
      scanner->skipWhiteSpace();
      BasicIdentifier* i = scanner->expect<BasicIdentifier>();
      severity = i->text;
      scanner->expect(";");
      return this;
    }

  }
}
