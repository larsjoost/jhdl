
#ifndef VHDL_PARSER_CASESTATEMENTALTERNATIVE_HPP_
#define VHDL_PARSER_CASESTATEMENTALTERNATIVE_HPP_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/case_statement_alternative.hpp"

namespace vhdl {
  namespace parser {

    class CaseStatementAlternative :
      public ::ast::CaseStatementAlternative  {

    public:

      CaseStatementAlternative* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
