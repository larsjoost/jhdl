
#ifndef VHDL_PARSER_INTERFACELIST_H_
#define VHDL_PARSER_INTERFACELIST_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/interface_list.hpp"
#include "interface_element.hpp"

namespace vhdl {
  namespace parser {

    template<scanner::Scanner::Keyword defaultType>
    class InterfaceList :
      public ::ast::InterfaceList  {

    public:

      InterfaceList<defaultType>* parse(::ast::Scanner<scanner::Scanner>* scanner) {
        scanner->accept("(");
        interfaceElements.add(scanner->expect<InterfaceElement<defaultType>>());
        while (scanner->optional(";")) {
          interfaceElements.add(scanner->expect<InterfaceElement<defaultType>>());
        };
        scanner->expect(")");
        return this;
      }

    };

  }

}
  

#endif
