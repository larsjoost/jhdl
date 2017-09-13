
#ifndef AST_SEQUENTIAL_STATEMENT_H_
#define AST_SEQUENTIAL_STATEMENT_H_

#include "procedure_call_statement.hpp"
#include "variable_assignment.hpp"
#include "signal_assignment.hpp"
#include "report_statement.hpp"
#include "if_statement.hpp"
#include "forloop_statement.hpp"
#include "whileloop_statement.hpp"
#include "wait_statement.hpp"
#include "return_statement.hpp"
#include "case_statement.hpp"

namespace ast {

  class IfStatement;
  class ForLoopStatement;
  class CaseStatement;
  
  class SequentialStatement {
    
  public:

    ProcedureCallStatement* procedureCallStatement;
    VariableAssignment* variableAssignment;
    SignalAssignment* signalAssignment;
    ReportStatement* reportStatement;
    IfStatement* ifStatement;
    ForLoopStatement* forLoopStatement;
    WhileLoopStatement* whileLoopStatement;
    WaitStatement* waitStatement;
    ReturnStatement* returnStatement;
    CaseStatement* caseStatement;
    
  };

}

#endif
