#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier.hpp"

namespace verilog {
  namespace parser {
  
    BasicIdentifier* BasicIdentifier::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->expect(::ast::TOKEN_IDENTIFIER);
      return this;
    }
    
  }
}

