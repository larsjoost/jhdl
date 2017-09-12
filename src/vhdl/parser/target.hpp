
#ifndef VHDL_TARGET_HPP_
#define VHDL_TARGET_HPP_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/target.hpp"

namespace vhdl {
  namespace parser {

    class Target :
      public ::ast::Target  {

    public:

      Target* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
