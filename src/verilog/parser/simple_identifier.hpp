#ifndef VERILOG_PARSER_SIMPLEIDENTIFIER_H_
#define VERILOG_PARSER_SIMPLEIDENTIFIER_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/simple_identifier.hpp"

namespace verilog {
  namespace parser {
    
    class SimpleIdentifier :
      public ::ast::SimpleIdentifier {
    
    public:

      SimpleIdentifier* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };
  }
}

#endif
