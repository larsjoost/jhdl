
#ifndef AST_REPORT_STATEMENT_H_
#define AST_REPORT_STATEMENT_H_

#include "basic_identifier.hpp"
#include "expression.hpp"

namespace ast {
    
  class ReportStatement {
    
  public:

    Expression* assert_condition;
    Expression* message;
    SimpleIdentifier* severity = NULL;
    
  };

}

#endif
