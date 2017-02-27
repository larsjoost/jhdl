
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "expression_term.hpp"
#include "basic_identifier.hpp"
#include "number.hpp"
#include "character.hpp"
#include "string.hpp"
#include "physical.hpp"
#include "procedure_call_statement.hpp"

namespace vhdl {
  namespace parser {
  
    ExpressionTerm* ExpressionTerm::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      (physical = scanner->optional<Physical>()) ||
        (number = scanner->optional<Number>()) ||
        (character = scanner->optional<Character>()) ||
        (text = scanner->optional<String>()) ||
        (procedureCall = scanner->optional<ProcedureCallStatement>()) || 
        (identifier = scanner->optional<BasicIdentifier>());
      return this;
    }

  }
}

