
#ifndef VHDL_PARSER_PROCEDURE_CALL_STATEMENT_H_
#define VHDL_PARSER_PROCEDURE_CALL_STATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/procedure_call_statement.hpp"

namespace vhdl {
  namespace parser {

    class ProcedureCallStatement :
      public ::ast::ProcedureCallStatement  {

    public:

      ProcedureCallStatement* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
