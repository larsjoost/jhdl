
#ifndef VHDL_VARIABLE_ASSIGNMENT_STATEMENT_H_
#define VHDL_VARIABLE_ASSIGNMENT_STATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/variable_assignment.hpp"

namespace vhdl {
  namespace parser {

    class VariableAssignment :
      public ::ast::VariableAssignment  {

    public:

      VariableAssignment* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
