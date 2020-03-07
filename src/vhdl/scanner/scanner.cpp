
#include "../../ast/token_scanner.hpp"

#include "scanner.hpp"

namespace vhdl {
  namespace scanner {
    
    int Scanner::skipWhiteSpaceAndComments(::ast::TokenScanner<Scanner>* scanner) {
      m_debug.functionStart("skipWhiteSpaceAndComments");
      int size = 0;
      int len  = scanner->skipWhiteSpace();
      do {
        size += len;
        try {
          scanner->accept("--");
          size += 2;
          size += scanner->skipUntilEndOfLine();
        } catch (ast::TokenNotAccepted e) {
        }
      } while (len = scanner->skipWhiteSpace());
      m_debug.functionEnd("skipWhiteSpaceAndComments = " + std::to_string(size));
      return size;
    }
    
  }
}

