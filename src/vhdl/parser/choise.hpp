
#ifndef VHDL_PARSER_CHOISE_HPP_
#define VHDL_PARSER_CHOISE_HPP_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/choise.hpp"

namespace vhdl {
  namespace parser {

    class Choise :
      public ::ast::Choise  {

    public:

      Choise* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
