#ifndef VHDL_PARSER_CHARACTER_H_
#define VHDL_PARSER_CHARACTER_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/character.hpp"

namespace vhdl {
  namespace parser {
    
    class Character :
      public ::ast::Character {
    
    public:

      Character* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };
  }
}

#endif
