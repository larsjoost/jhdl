
#ifndef AST_REPORT_STATEMENT_H_
#define AST_REPORT_STATEMENT_H_

#include "text.hpp"
#include "expression.hpp"

namespace ast {
    
  class ReportStatement {
    
  public:

    Expression* message;
    Text severity;
    
  };

}

#endif
