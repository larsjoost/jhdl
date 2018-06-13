
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "association_list.hpp"
#include "association_element.hpp"

namespace vhdl {
  namespace parser {
  
    AssociationList* AssociationList::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      associationElements.add(scanner->accept<AssociationElement>());
      while (scanner->optional(",")) {
        associationElements.add(scanner->expect<AssociationElement, AssociationList>());
      };
      return this;
    }

  }
}

