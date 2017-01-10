
#ifndef VHDL_PARSER_ENTITY_H_
#define VHDL_PARSER_ENTITY_H_

#include "../scanner/scanner.hpp"
#include "../ast/entity.hpp"

namespace vhdl {
  namespace parser {
  
    class Entity :
      public ast::Entity {
	
    public:

      void parse(scanner::Scanner* s);

    };
    
  }
}

#endif

