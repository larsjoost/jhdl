
#ifndef VHDL_PARSER_CONSTANTDECLARATION_H_
#define VHDL_PARSER_CONSTANTDECLARATION_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/constant_declaration.hpp"

namespace vhdl {
  namespace parser {

    class ConstantDeclaration :
      public ::ast::ConstantDeclaration  {

    public:

      ConstantDeclaration* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
