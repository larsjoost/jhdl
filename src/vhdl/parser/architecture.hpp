
#ifndef VHDL_PARSER_ARCHITECTURE_H_
#define VHDL_PARSER_ARCHITECTURE_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/implementation.hpp"

namespace vhdl {
  namespace parser {

    class Architecture :
      public ::ast::Implementation  {

    public:

      Architecture* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
