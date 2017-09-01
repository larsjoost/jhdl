
#ifndef AST_CASE_STATEMENT_CHOISE_HPP_
#define AST_CASE_STATEMENT_CHOISE_HPP_

#include "scalar_type.hpp"

namespace ast {

  class CaseStatementChoise {
    
  public:
    
    ScalarType* scalar_type = NULL;
    bool others = false;
    
  };

}

#endif
