#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "character.hpp"

namespace vhdl {
  namespace parser {
  
    Character* Character::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      ::ast::Text* v = scanner->accept(::ast::TOKEN_CHARACTER);
      value = *v;
      return this;
    }
    
  }
}

