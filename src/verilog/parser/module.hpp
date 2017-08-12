
#ifndef VERILOG_PARSER_ENTITY_H_
#define VERILOG_PARSER_ENTITY_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/module.hpp"
#include "../../debug/debug.hpp"

namespace verilog {
  namespace parser {
  
    class Module : public ::ast::Module {

      Debug<false> debug;
      
    public:

      Module() : debug("verilog::parser::Module") {};
      
      Module* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };
    
  }
}

#endif

