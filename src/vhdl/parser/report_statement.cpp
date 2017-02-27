#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "report_statement.hpp"
#include "basic_identifier.hpp"
#include "expression.hpp"

namespace vhdl {
  namespace parser {
  
    ReportStatement* ReportStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_REPORT);
      message = scanner->expect<Expression>();
      scanner->expect(scanner::Scanner::VHDL_SEVERITY);
      severity = scanner->expect<BasicIdentifier>();
      return this;
    }

  }
}
