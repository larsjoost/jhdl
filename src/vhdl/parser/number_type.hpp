
#ifndef VHDL_PARSER_NUMBERTYPE_H_
#define VHDL_PARSER_NUMBERTYPE_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/type_declaration.hpp"

namespace vhdl {
  namespace parser {

    class NumberType :
      public ::ast::NumberType  {

    public:

      NumberType* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
