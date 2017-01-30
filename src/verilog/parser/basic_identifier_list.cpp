#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier_list.hpp"
#include "basic_identifier.hpp"

namespace verilog {
  namespace parser {
  
    void BasicIdentifierList::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      textList.add(scanner->expect<BasicIdentifier>());
      scanner->skipWhiteSpace();
      while (scanner->optional(",")) {
        scanner->skipWhiteSpace();
        textList.add(scanner->expect<BasicIdentifier>());
        scanner->skipWhiteSpace();
      };
    }
    
  }
}

