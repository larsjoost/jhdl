#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "number.hpp"

namespace vhdl {
  namespace parser {
  
    Number* Number::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      ::ast::Text* v = scanner->accept(::ast::TOKEN_NUMBER);
      value = *v;
      return this;
    }
    
  }
}

