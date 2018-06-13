
#ifndef VHDL_PARSER_INTERFACELIST_H_
#define VHDL_PARSER_INTERFACELIST_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/interface_list.hpp"
#include "interface_element.hpp"

namespace vhdl {
  namespace parser {

    template<scanner::Scanner::Keyword defaultType, bool parenthis = true>
    class InterfaceList :
      public ::ast::InterfaceList  {

    public:

      InterfaceList<defaultType, parenthis>* parse(::ast::Scanner<scanner::Scanner>* scanner) {
        if (parenthis) {
          scanner->accept("(");
          interfaceElements.add(scanner->expect<InterfaceElement<defaultType>, InterfaceList>());
          while (scanner->optional(";")) {
            interfaceElements.add(scanner->expect<InterfaceElement<defaultType>, InterfaceList>());
          };
          scanner->expect(")");
        } else {
          while (interfaceElements.add(scanner->accept<InterfaceElement<defaultType>>())) {
            scanner->expect(";");
          };
        }
        return this;
      }

    };

  }

}
  

#endif
