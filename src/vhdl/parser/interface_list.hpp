
#ifndef VHDL_PARSER_INTERFACELIST_H_
#define VHDL_PARSER_INTERFACELIST_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/interface_list.hpp"

namespace vhdl {
  namespace parser {

    class InterfaceList :
      public ::ast::InterfaceList  {

    public:

      InterfaceList* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
