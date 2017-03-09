
#ifndef VHDL_PARSER_ARRAYTYPE_H_
#define VHDL_PARSER_ARRAYTYPE_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/array_type.hpp"

namespace vhdl {
  namespace parser {

    class ArrayType :
      public ::ast::ArrayType  {

    public:

      ArrayType* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
