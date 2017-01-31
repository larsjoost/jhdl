#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier_list.hpp"
#include "basic_identifier.hpp"

namespace verilog {
  namespace parser {
  
    void BasicIdentifierList::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      textList.add(scanner->expect<BasicIdentifier>());
      while (scanner->optional(",")) {
        textList.add(scanner->expect<BasicIdentifier>());
      };
    }
    
  }
}

