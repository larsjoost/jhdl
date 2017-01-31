
#ifndef VHDL_IF_STATEMENT_H_
#define VHDL_IF_STATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/if_statement.hpp"

namespace vhdl {
  namespace parser {

    class IfStatement :
      public ::ast::IfStatement  {

    public:

      IfStatement* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
