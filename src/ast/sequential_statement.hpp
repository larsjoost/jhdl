
#ifndef AST_SEQUENTIAL_STATEMENT_H_
#define AST_SEQUENTIAL_STATEMENT_H_

#include "procedure_call_statement.hpp"
#include "variable_assignment.hpp"
#include "signal_assignment.hpp"
#include "report_statement.hpp"
#include "if_statement.hpp"
#include "forloop_statement.hpp"
#include "loop_statement.hpp"
#include "exit_statement.hpp"
#include "next_statement.hpp"
#include "whileloop_statement.hpp"
#include "wait_statement.hpp"
#include "return_statement.hpp"
#include "case_statement.hpp"

namespace ast {

  class IfStatement;
  class ForLoopStatement;
  class LoopStatement;
  class ExitStatement;
  class NextStatement;
  class CaseStatement;
  
  class SequentialStatement {
    
  public:

    ProcedureCallStatement* procedureCallStatement = NULL;
    VariableAssignment* variableAssignment = NULL;
    SignalAssignment* signalAssignment = NULL;
    ReportStatement* reportStatement = NULL;
    IfStatement* ifStatement = NULL;
    ForLoopStatement* forLoopStatement = NULL;
    LoopStatement* loopStatement = NULL;
    ExitStatement* exitStatement = NULL;
    NextStatement* nextStatement = NULL;
    WhileLoopStatement* whileLoopStatement = NULL;
    WaitStatement* waitStatement = NULL;
    ReturnStatement* returnStatement = NULL;
    CaseStatement* caseStatement = NULL;
    
  };

}

#endif
