
#ifndef VHDL_PARSER_PHYSICALTYPE_H_
#define VHDL_PARSER_PHYSICALTYPE_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/type_declaration.hpp"

namespace vhdl {
  namespace parser {

    class PhysicalType :
      public ::ast::PhysicalType  {

    public:

      PhysicalType* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
