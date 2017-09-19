
#ifndef VHDL_PARSER_EXITSTATEMENT_H_
#define VHDL_PARSER_EXITSTATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/exit_statement.hpp"

namespace vhdl {
  namespace parser {

    class ExitStatement :
      public ::ast::ExitStatement  {

    public:

      ExitStatement* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
