#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "case_statement_alternative.hpp"
#include "case_statement_choise.hpp"
#include "sequential_statement.hpp"

namespace vhdl {
  namespace parser {
  
    CaseStatementAlternative* CaseStatementAlternative::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept(scanner::Scanner::VHDL_WHEN);
      do {
        choises.add(scanner->expect<CaseStatementChoise>());
      } while(scanner->optional("|"));
      scanner->expect("=>");
      while (sequentialStatements.add(scanner->optional<SequentialStatement>())) {};
      return this;
    }

  }
}
