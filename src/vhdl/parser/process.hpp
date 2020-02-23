
#ifndef VHDL_PARSER_PROCESS_H_
#define VHDL_PARSER_PROCESS_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/process.hpp"
#include "../../ast/procedure_call_statement.hpp"
#include "../../ast/report_statement.hpp"

namespace vhdl {
  namespace parser {

    class Process :
      public ::ast::Process  {

    public:

      Process* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
