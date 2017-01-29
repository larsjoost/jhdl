#ifndef VHDL_PARSER_STRING_HPP_
#define VHDL_PARSER_STRING_HPP_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/string.hpp"

namespace vhdl {
  namespace parser {
    
    class String :
      public ::ast::String {
    
    public:

      String* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };
  }
}

#endif
