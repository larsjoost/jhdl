
#ifndef VHDL_PARSER_WAITSTATEMENT_H_
#define VHDL_PARSER_WAITSTATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/wait_statement.hpp"

namespace vhdl {
  namespace parser {

    class WaitStatement :
      public ::ast::WaitStatement  {

    public:

      WaitStatement* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
