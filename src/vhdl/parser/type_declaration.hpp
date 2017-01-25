
#ifndef VHDL_PARSER_TYPEDECLARATION_H_
#define VHDL_PARSER_TYPEDECLARATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/type_declaration.hpp"

namespace vhdl {
  namespace parser {

    class TypeDeclaration :
      public ::ast::TypeDeclaration  {

    public:

      TypeDeclaration* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
