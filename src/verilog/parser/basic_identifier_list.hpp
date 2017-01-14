#ifndef VERILOG_PARSER_BASICIDENTIFIERLIST_H_
#define VERILOG_PARSER_BASICIDENTIFIERLIST_H_

#include "../scanner/scanner.hpp"
#include "../../ast/basic_identifier_list.hpp"

namespace verilog {
  namespace parser {
    
    class BasicIdentifierList :
      public ::ast::BasicIdentifierList {
    
    public:

      void parse(scanner::Scanner* scanner);
      
    };
  }
}

#endif
