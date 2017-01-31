#ifndef VHDL_PARSER_BASICIDENTIFIER_H_
#define VHDL_PARSER_BASICIDENTIFIER_H_

#include <string>
#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/basic_identifier.hpp"

namespace vhdl {
  namespace parser {
    
    class BasicIdentifier :
      public ::ast::BasicIdentifier {
    
    public:

      void parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };
  }
}

#endif
