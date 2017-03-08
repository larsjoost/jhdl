
#ifndef VHDL_PARSER_SUBTYPEDECLARATION_H_
#define VHDL_PARSER_SUBTYPEDECLARATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/subtype_declaration.hpp"

namespace vhdl {
  namespace parser {

    class SubtypeDeclaration :
      public ::ast::SubtypeDeclaration  {

    public:

      SubtypeDeclaration* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
