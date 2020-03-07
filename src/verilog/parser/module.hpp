
#ifndef VERILOG_PARSER_ENTITY_H_
#define VERILOG_PARSER_ENTITY_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/module.hpp"
#include "../../debug/debug.hpp"

namespace verilog {
  namespace parser {
  
    class Module : public ::ast::Module {

      Debug<false> m_debug;
      
    public:

      Module() : m_debug(this) {};
      
      Module* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };
    
  }
}

#endif

