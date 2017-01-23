
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "association_list.hpp"
#include "association_element.hpp"

namespace vhdl {
  namespace parser {
  
    AssociationList* AssociationList::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->skipWhiteSpace();
      associationElements.add(scanner->expect<AssociationElement>());
      scanner->skipWhiteSpace();
      while (scanner->optional(",")) {
        scanner->skipWhiteSpace();
        associationElements.add(scanner->expect<AssociationElement>());
        scanner->skipWhiteSpace();
      };
      return this;
    }

  }
}

