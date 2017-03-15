
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "expression_term.hpp"
#include "basic_identifier.hpp"
#include "number.hpp"
#include "character.hpp"
#include "string.hpp"
#include "physical.hpp"

namespace vhdl {
  namespace parser {
  
    ExpressionTerm* ExpressionTerm::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      (physical = scanner->optional<Physical>()) ||
        (number = scanner->optional<Number>()) ||
        (character = scanner->optional<Character>()) ||
        (text = scanner->optional<String>()) || 
        (identifier = scanner->optional<BasicIdentifier>());
      return this;
    }

  }
}

