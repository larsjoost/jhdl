#ifndef VERILOG_PARSER_BASICIDENTIFIER_H_
#define VERILOG_PARSER_BASICIDENTIFIER_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/basic_identifier.hpp"

namespace verilog {
  namespace parser {
    
    class BasicIdentifier :
      public ::ast::BasicIdentifier {
    
    public:

      void parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };
  }
}

#endif
