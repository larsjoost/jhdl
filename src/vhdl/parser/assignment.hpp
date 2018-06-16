
#ifndef VHDL_ASSIGNMENT_STATEMENT_H_
#define VHDL_ASSIGNMENT_STATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/assignment.hpp"

namespace vhdl {
  namespace parser {

    class Assignment :
      public ::ast::Assignment  {

    public:

      Assignment* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
