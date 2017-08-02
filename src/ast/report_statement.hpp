
#ifndef AST_REPORT_STATEMENT_H_
#define AST_REPORT_STATEMENT_H_

#include "basic_identifier.hpp"
#include "expression.hpp"

namespace ast {
    
  class ReportStatement {
    
  public:

    SimpleIdentifier* assert_condition;
    Expression* message;
    SimpleIdentifier* severity;
    
  };

}

#endif
