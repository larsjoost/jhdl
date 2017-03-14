#include "../../ast/scanner.hpp"
#include "../../ast/text.hpp"
#include "../scanner/scanner.hpp"
#include "simple_identifier.hpp" 

namespace vhdl {
  namespace parser {
  
    void SimpleIdentifier::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      text = *(scanner->accept(::ast::TOKEN_IDENTIFIER));
    }

  }
}

