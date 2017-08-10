
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "scalar_type.hpp"
#include "simple_identifier.hpp"
#include "number.hpp"
#include "character.hpp"
#include "string.hpp"
#include "physical.hpp"

namespace vhdl {
  namespace parser {
  
    ScalarType* ScalarType::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      text = scanner->getCurrentTextPosition();
      (physical = scanner->optional<Physical>()) ||
        (number = scanner->optional<Number>()) ||
        (character = scanner->optional<Character>()) ||
        (string = scanner->optional<String>()) || 
        (identifier = scanner->optional<SimpleIdentifier>());
      return this;
    }

  }
}

