
#ifndef VERILOG_PARSER_ENTITY_H_
#define VERILOG_PARSER_ENTITY_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/module.hpp"

namespace verilog {
  namespace parser {
  
    class Module :
      public ::ast::Module {
	
    public:

      Module* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };
    
  }
}

#endif

