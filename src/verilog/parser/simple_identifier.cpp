
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "simple_identifier.hpp"

namespace verilog {
  namespace parser {
  
    SimpleIdentifier* SimpleIdentifier::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->expect(::ast::TOKEN_IDENTIFIER);
      return this;
    }
    
  }
}

