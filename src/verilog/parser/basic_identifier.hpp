#ifndef VERILOG_PARSER_BASICIDENTIFIER_H_
#define VERILOG_PARSER_BASICIDENTIFIER_H_

#include "../scanner/scanner.hpp"
#include "../../ast/basic_identifier.hpp"

namespace verilog {
  namespace parser {
    
    class BasicIdentifier :
      public ::ast::BasicIdentifier {
    
    public:

      void parse(scanner::Scanner* scanner);
      
    };
  }
}

#endif
