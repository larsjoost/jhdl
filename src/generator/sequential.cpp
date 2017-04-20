#include "systemc.hpp"
#include "sequential.hpp"
#include "expression/expression.hpp"

namespace generator {

  std::string SystemC::procedureCallStatementToString(parameters& parm, ast::ProcedureCallStatement* p) {
    std::string s = "";
    if (p) {
      functionStart("procedureCallStatementToString");
      s = p->name->toString(true);
      functionEnd("procedureCallStatementToString");
    }
    return s;
  }

  void SystemC::procedureCallStatement(parameters& parm, ast::ProcedureCallStatement* p) {
    if (p) {
      println(parm, procedureCallStatementToString(parm, p) + ";");
    }
  }
  
  void SystemC::variableAssignment(parameters& parm, ast::VariableAssignment* p) {
    if (p) {
      printSourceLine(parm, p->identifier);
      ExpressionParser expr(&database);
      println(parm, p->identifier->toString(true) + " = " +
              expr.toString(p->expression) + ";");
    }
  }

  void SystemC::signalAssignment(parameters& parm, ast::SignalAssignment* p) {
    signalAssignment(parm, p,  [](DatabaseResult& object) {});
  }


  void SystemC::reportStatement(parameters& parm, ast::ReportStatement* p) {
    if (p) {
      ExpressionParser expr(&database);
      println(parm, "report(" + expr.toString(p->message) + ", " +
              p->severity->toString() + ");");
    }
  }

    void SystemC::ifStatement(parameters& parm, ast::IfStatement* p) {
    if (p) {
      std::string command = "if ";
      ExpressionParser expr(&database);
      for (::ast::ConditionalStatement c : p->conditionalStatements.list) {
	if (c.condition) {
	  println(parm, command + " (" + expr.toString(c.condition) + ") {");
	} else {
	  println(parm, "} else {");
	}
	command = "} elsif";
        parm.incIndent();
        sequentialStatements(parm, c.sequentialStatements);
        parm.decIndent();
      }
      println(parm, "}");
    }
  }

  void SystemC::forLoopStatement(parameters& parm, ast::ForLoopStatement* f) {
    if (f) {
      assert(f->identifier);
      std::string name = f->identifier->toString(true);
      descendHierarchy(parm);
      database.add(ast::VARIABLE, name, ast::INTEGER);
      forLoop(parm, name, f->range, [&](parameters& parm) {
          sequentialStatements(parm, f->sequentialStatements);
        });
      ascendHierarchy(parm);
    }
  }
  
  void SystemC::waitStatement(parameters& parm, ast::WaitStatement* p) {
    if (p) {
      println(parm, "wait(" + numberToString(parm, p->physical->number) + ");");
    }
  }


  void SystemC::returnStatement(parameters& parm, ast::ReturnStatement* r) {
    if (r) {
      ExpressionParser expr(&database);
      println(parm, "return " + expr.toString(r->value) + ";");
    }
  }

  void SystemC::sequentialStatements(parameters& parm, ast::List<ast::SequentialStatement>& l) {
    for (ast::SequentialStatement s : l.list) {
      procedureCallStatement(parm, s.procedureCallStatement);
      variableAssignment(parm, s.variableAssignment);
      signalAssignment(parm, s.signalAssignment);
      reportStatement(parm, s.reportStatement);
      ifStatement(parm, s.ifStatement);
      forLoopStatement(parm, s.forLoopStatement);
      waitStatement(parm, s.waitStatement);
      returnStatement(parm, s.returnStatement);
    }
  }

}
