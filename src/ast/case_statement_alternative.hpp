
#ifndef AST_CASE_STATEMENT_ALTERNATIVE_HPP_
#define AST_CASE_STATEMENT_ALTERNATIVE_HPP_

#include "simple_identifier.hpp"
#include "list.hpp"
#include "case_statement_alternative.hpp"
#include "sequential_statement.hpp"
#include "scalar_type.hpp"

namespace ast {

  class CaseStatementAlternative {
    
  public:
    
    List<ScalarType> choises;
    bool others = false;
    List<SequentialStatement> sequentialStatements;
    
  };

}

#endif
