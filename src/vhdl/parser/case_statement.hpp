
#ifndef VHDL_PARSER_CASESTATEMENT_HPP_
#define VHDL_PARSER_CASESTATEMENT_HPP_

#include "../../ast/scanner.hpp"
#include "../scanner/scanner.hpp"
#include "../../ast/case_statement.hpp"

namespace vhdl {
  namespace parser {

    class CaseStatement :
      public ::ast::CaseStatement  {

    public:

      CaseStatement* parse(::ast::Scanner<scanner::Scanner>* scanner);
      
    };

  }

}
  

#endif
