#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "basic_identifier_list.hpp"
#include "basic_identifier.hpp"

namespace verilog {
  namespace parser {
  
    void BasicIdentifierList::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      BasicIdentifier* i;
      i = scanner->expect<BasicIdentifier>();
      textList.add(&i->text);
      scanner->skipWhiteSpace();
      while (scanner->optional(",")) {
        scanner->skipWhiteSpace();
        i = scanner->expect<BasicIdentifier>();
        textList.add(&i->text);
        scanner->skipWhiteSpace();
      };
    }
    
  }
}

