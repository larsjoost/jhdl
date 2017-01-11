#include "../scanner/scanner.hpp"
#include "basic_identifier_list.hpp"
#include "basic_identifier.hpp"

namespace vhdl {
  namespace parser {
  
    void BasicIdentifierList::parse(scanner::Scanner* scanner) {
      scanner->skipWhiteSpaceAndComments();
      BasicIdentifier* i;
      i = scanner->expect<BasicIdentifier>();
      textList.add(&i->text);
      scanner->skipWhiteSpaceAndComments();
      while (scanner->optional(".")) {
        scanner->skipWhiteSpaceAndComments();
        i = scanner->expect<BasicIdentifier>();
        textList.add(&i->text);
        scanner->skipWhiteSpaceAndComments();
      };
    }
    
  }
}

