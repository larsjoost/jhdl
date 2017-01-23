#ifndef VHDL_PARSER_BASICIDENTIFIERLIST_H_
#define VHDL_PARSER_BASICIDENTIFIERLIST_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/basic_identifier_list.hpp"

namespace vhdl {
  namespace parser {
    
    class BasicIdentifierList :
      public ::ast::BasicIdentifierList {
    
    public:

      void parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };
  }
}

#endif
