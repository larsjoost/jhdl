

#include "../../ast/scanner.hpp"
#include "../../ast/sequential_statement.hpp"
#include "../scanner/scanner.hpp"
#include "sequential_statement.hpp"
#include "procedure_call_statement.hpp"
#include "variable_assignment.hpp"
#include "report_statement.hpp"
#include "declaration.hpp"
#include "if_statement.hpp"
#include "forloop_statement.hpp"
#include "wait_statement.hpp"

namespace vhdl {
  namespace parser {
  
    SequentialStatement* SequentialStatement::parse(::ast::Scanner<scanner::Scanner>* scanner) {
      (variableAssignment = scanner->optional<VariableAssignment>()) ||
	(reportStatement = scanner->optional<ReportStatement>()) ||
        (procedureCallStatement = scanner->optional<ProcedureCallStatement>()) ||
	(ifStatement = scanner->optional<IfStatement>()) ||
	(forLoopStatement = scanner->optional<ForLoopStatement>()) ||
	(waitStatement = scanner->optional<WaitStatement>());
      return this;
    }
    
  }
}

