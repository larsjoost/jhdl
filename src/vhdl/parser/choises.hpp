
#ifndef VHDL_PARSER_CHOISES_HPP_
#define VHDL_PARSER_CHOISES_HPP_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/choises.hpp"

namespace vhdl {
  namespace parser {

    class Choises :
      public ::ast::Choises  {

    public:

      Choises* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
