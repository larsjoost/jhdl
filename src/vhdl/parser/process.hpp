
#ifndef VHDL_PARSER_PROCESS_H_
#define VHDL_PARSER_PROCESS_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/method.hpp"
#include "../../ast/procedure_call_statement.hpp"
#include "../../ast/report_statement.hpp"

namespace vhdl {
  namespace parser {

    class Process :
      public ::ast::Method  {

    protected:

      bool add(::ast::ProcedureCallStatement* p);
      bool add(::ast::VariableAssignment* p);
      bool add(::ast::ReportStatement* p);

      void parseDeclarations(::ast::Scanner<scanner::Scanner>* scanner);
      void parseBody(::ast::Scanner<scanner::Scanner>* scanner);

    public:

      Process* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
