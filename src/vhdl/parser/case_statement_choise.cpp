#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "scalar_type.hpp"
#include "case_statement_choise.hpp"

namespace vhdl {
  namespace parser {
  
    CaseStatementChoise* CaseStatementChoise::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      (
       (scalar_type = scanner->optional<ScalarType>()) ||
       (others = scanner->optional(scanner::Scanner::VHDL_OTHERS)));
      return this;
    }

  }
}
