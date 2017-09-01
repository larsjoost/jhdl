
#ifndef AST_CASE_STATEMENT_ALTERNATIVE_HPP_
#define AST_CASE_STATEMENT_ALTERNATIVE_HPP_

#include "list.hpp"
#include "case_statement_choise.hpp"
#include "sequential_statement.hpp"

namespace ast {

  class CaseStatementAlternative {
    
  public:
    
    List<CaseStatementChoise> choises;
    bool others = false;
    List<SequentialStatement> sequentialStatements;
    
  };

}

#endif
