
#ifndef VERILOG_PARSER_ENTITY_H_
#define VERILOG_PARSER_ENTITY_H_

#include "../scanner/scanner.hpp"
#include "../../ast/architecture.hpp"

namespace verilog {
  namespace parser {
  
    class Module :
      public ::ast::Architecture {
	
    public:

      Module* parse(scanner::Scanner* s);

    };
    
  }
}

#endif

