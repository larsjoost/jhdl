#ifndef VHDL_PARSER_PHYSICAL_H_
#define VHDL_PARSER_PHYSICAL_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/physical.hpp"

namespace vhdl {
  namespace parser {
    
    class Physical :
      public ::ast::Physical {
    
    public:

      Physical* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };
  }
}

#endif
