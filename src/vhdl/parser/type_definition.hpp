
#ifndef VHDL_PARSER_TYPEDEFINITION_H_
#define VHDL_PARSER_TYPEDEFINITION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/type_definition.hpp"

namespace vhdl {
  namespace parser {

    class TypeDefinition :
      public ::ast::TypeDefinition  {

    public:

      TypeDefinition* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
