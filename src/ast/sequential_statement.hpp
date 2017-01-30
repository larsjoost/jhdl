
#ifndef AST_SEQUENTIAL_STATEMENT_H_
#define AST_SEQUENTIAL_STATEMENT_H_

#include "text.hpp"
#include "list.hpp"
#include "procedure_call_statement.hpp"
#include "variable_assignment.hpp"
#include "report_statement.hpp"

namespace ast {
    
  class SequentialStatement {
    
  public:

    ProcedureCallStatement* procedureCallStatement;
    VariableAssignment* variableAssignment;
    ReportStatement* reportStatement;
    
  };

}

#endif