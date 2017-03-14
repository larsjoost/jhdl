#ifndef VHDL_PARSER_SIMPLEIDENTIFIER_H_
#define VHDL_PARSER_SIMPLEIDENTIFIER_H_

#include <string>
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/simple_identifier.hpp"

namespace vhdl {
  namespace parser {
    
    class SimpleIdentifier :
      public ::ast::SimpleIdentifier {
      
    public:

      void parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };
  }
}

#endif
