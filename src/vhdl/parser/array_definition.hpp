
#ifndef VHDL_PARSER_ARRAYDEFINITION_H_
#define VHDL_PARSER_ARRAYDEFINITION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/array_definition.hpp"

namespace vhdl {
  namespace parser {

    class ArrayDefinition :
      public ::ast::ArrayDefinition  {

    public:

      ArrayDefinition* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
