#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "number.hpp"

namespace vhdl {
  namespace parser {
  
    Number* Number::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      scanner->expect(::ast::TOKEN_NUMBER);
      return this;
    }
    
  }
}

