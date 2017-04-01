
#ifndef VHDL_PARSER_ARRAYSUBTYPEDEFINITION_H_
#define VHDL_PARSER_ARRAYSUBTYPEDEFINITION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/array_subtype_definition.hpp"

namespace vhdl {
  namespace parser {

    class ArraySubtypeDefinition :
      public ::ast::ArraySubtypeDefinition  {

    public:

      ArraySubtypeDefinition* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
