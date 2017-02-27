
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../scanner/defines.hpp"
#include "interface_list.hpp"
#include "interface_element.hpp"

namespace vhdl {
  namespace parser {
  
    InterfaceList* InterfaceList::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      scanner->accept("(");
      interfaceElements.add(scanner->expect<InterfaceElement>());
      while (scanner->optional(";")) {
        interfaceElements.add(scanner->expect<InterfaceElement>());
      };
      scanner->expect(")");
      return this;
    }

  }
}

