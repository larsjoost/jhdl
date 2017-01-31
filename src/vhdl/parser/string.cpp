#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "string.hpp"

namespace vhdl {
  namespace parser {
  
    String* String::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      ::ast::Text* v = scanner->accept(::ast::TOKEN_STRING);
      text = *v;
      return this;
    }
    
  }
}

