
#ifndef VHDL_REPORT_STATEMENT_H_
#define VHDL_REPORT_STATEMENT_H_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/report_statement.hpp"

namespace vhdl {
  namespace parser {

    class ReportStatement :
      public ::ast::ReportStatement  {

    public:

      ReportStatement* parse(::ast::Scanner<scanner::Scanner>* scanner);

    };

  }

}
  

#endif
