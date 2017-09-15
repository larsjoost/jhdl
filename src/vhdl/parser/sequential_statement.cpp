

#include "../../ast/scanner.hpp"
#include "../../ast/sequential_statement.hpp"
#include "../scanner/scanner.hpp"
#include "sequential_statement.hpp"
#include "procedure_call_statement.hpp"
#include "variable_assignment.hpp"
#include "signal_assignment.hpp"
#include "report_statement.hpp"
#include "declaration.hpp"
#include "if_statement.hpp"
#include "forloop_statement.hpp"
#include "whileloop_statement.hpp"
#include "wait_statement.hpp"
#include "return_statement.hpp"
#include "case_statement.hpp"

namespace vhdl {
  namespace parser {
  
    SequentialStatement* SequentialStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      if ((variableAssignment = scanner->optional<VariableAssignment>()) ||
          (signalAssignment = scanner->optional<SignalAssignment>()) ||
          (reportStatement = scanner->optional<ReportStatement>()) ||
          (caseStatement = scanner->optional<CaseStatement>()) ||
          (forLoopStatement = scanner->optional<ForLoopStatement>()) ||
          (procedureCallStatement = scanner->optional<ProcedureCallStatement>()) ||
          (ifStatement = scanner->optional<IfStatement>()) ||
          (whileLoopStatement = scanner->optional<WhileLoopStatement>()) ||
          (waitStatement = scanner->optional<WaitStatement>()) ||
          (returnStatement = scanner->optional<ReturnStatement>())) {
        scanner->expect(";");
      }
      return this;
    }
    
  }
}

