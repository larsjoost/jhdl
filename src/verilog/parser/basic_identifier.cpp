#include "../scanner/scanner.hpp"
#include "basic_identifier.hpp"

namespace verilog {
  namespace parser {
  
    void BasicIdentifier::parse(scanner::Scanner* scanner) {
      scanner->skipWhiteSpaceAndComments();
      scanner::BasicIdentifier b;
      scanner->expect(b);
      text = b.text;
    }
    
  }
}

