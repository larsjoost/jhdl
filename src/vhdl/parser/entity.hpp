
#ifndef VHDL_PARSER_ENTITY_H_
#define VHDL_PARSER_ENTITY_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/interface.hpp"

namespace vhdl {
  namespace parser {
  
    class Entity :
      public ::ast::Interface {
	
    public:

      Entity* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };
    
  }
}

#endif

