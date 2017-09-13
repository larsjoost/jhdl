
#ifndef VHDL_TARGET_ARGUMENT_HPP_
#define VHDL_TARGET_ARGUMENT_HPP_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/target_argument.hpp"

namespace vhdl {
  namespace parser {

    class TargetArgument :
      public ::ast::TargetArgument  {

    public:

      TargetArgument* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
