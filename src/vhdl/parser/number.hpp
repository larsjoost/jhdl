#ifndef VHDL_PARSER_NUMBER_H_
#define VHDL_PARSER_NUMBER_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/number.hpp"

namespace vhdl {
  namespace parser {
    
    class Number :
      public ::ast::Number {
    
    public:

      Number* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };
  }
}

#endif
