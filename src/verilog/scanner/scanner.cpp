
#include "../../ast/token_scanner.hpp"
#include "scanner.hpp"

namespace verilog {
  namespace scanner {

    int Scanner::skipWhiteSpaceAndComments(::ast::TokenScanner<Scanner>* scanner) {
      int size = 0;
      int len;
      while (len = scanner->skipWhiteSpace()) {
        size += len;
        try {
          scanner->accept("//");
          size += 2;
          size += scanner->skipUntilEndOfLine();
        } catch (ast::TokenNotAccepted e) {
        }
      }
      return size;
    }
    
  }
}

