
#ifndef VHDL_SIGNAL_ASSIGNMENT_STATEMENT_H_
#define VHDL_SIGNAL_ASSIGNMENT_STATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/signal_assignment.hpp"

namespace vhdl {
  namespace parser {

    class SignalAssignment :
      public ::ast::SignalAssignment  {

    public:

      SignalAssignment* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
