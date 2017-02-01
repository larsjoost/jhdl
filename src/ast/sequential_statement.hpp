
#ifndef AST_SEQUENTIAL_STATEMENT_H_
#define AST_SEQUENTIAL_STATEMENT_H_

#include "procedure_call_statement.hpp"
#include "variable_assignment.hpp"
#include "report_statement.hpp"
#include "if_statement.hpp"
#include "forloop_statement.hpp"

namespace ast {

  class IfStatement;
  class ForLoopStatement;
  
  class SequentialStatement {
    
  public:

    ProcedureCallStatement* procedureCallStatement;
    VariableAssignment* variableAssignment;
    ReportStatement* reportStatement;
    IfStatement* ifStatement;
    ForLoopStatement* forLoopStatement;
    
  };

}

#endif
