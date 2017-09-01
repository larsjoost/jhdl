
#ifndef VHDL_PARSER_CASESTATEMENTCHOISE_HPP_
#define VHDL_PARSER_CASESTATEMENTCHOISE_HPP_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/case_statement_choise.hpp"

namespace vhdl {
  namespace parser {

    class CaseStatementChoise :
      public ::ast::CaseStatementChoise  {

    public:

      CaseStatementChoise* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
