
#ifndef VHDL_PARSER_COMPONENTINSTANCE_H_
#define VHDL_PARSER_COMPONENTINSTANCE_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/component_instance.hpp"

namespace vhdl {
  namespace parser {
  
    class ComponentInstance :
      public ::ast::ComponentInstance {
	
    public:

      ComponentInstance* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };
    
  }
}

#endif

