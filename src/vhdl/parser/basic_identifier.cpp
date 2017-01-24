#include "../../ast/scanner.hpp"
#include "../../ast/text.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    void BasicIdentifier::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      ::ast::Text* name = scanner->accept(::ast::TOKEN_IDENTIFIER);
      text = *name;
    }
    
  }
}

