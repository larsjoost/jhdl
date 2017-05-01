#include "systemc.hpp"
#include "sequential.hpp"
#include "expression/expression.hpp"

namespace generator {

  void SystemC::procedureCallStatement(parameters& parm, ast::ProcedureCallStatement* p) {
    if (p) {
      ExpressionParser expr(&database);
      println(parm, expr.procedureCallStatementToString(p) + ";");
    }
  }
  
  void SystemC::variableAssignment(parameters& parm, ast::VariableAssignment* p) {
    if (p) {
      printSourceLine(parm, p->identifier);
      ExpressionParser expr(&database);
      std::string name = p->identifier->toString(true);
      auto valid = [&](DatabaseElement* e) { return e->id == ast::VARIABLE; };
      DatabaseResult object;
      if (database.findOne(object, name, valid)) {
	println(parm, name + " = " + expr.toString(p->expression, object.object->type) + ";");
      } else {
	exceptions.printError("Cound to find definition of VARIABLE with name " + name, &p->identifier->text);
	database.printAllObjects(name);
      }
    }
  }

  void SystemC::signalAssignment(parameters& parm, ast::SignalAssignment* p) {
    signalAssignment(parm, p,  [](DatabaseResult& object) {});
  }


  void SystemC::reportStatement(parameters& parm, ast::ReportStatement* p) {
    if (p) {
      ExpressionParser expr(&database);
      ast::ObjectValueContainer expectedType("STRING");
      println(parm, "report(" + expr.toString(p->message, expectedType) + ", " +
              p->severity->toString() + ");");
    }
  }

    void SystemC::ifStatement(parameters& parm, ast::IfStatement* p) {
    if (p) {
      std::string command = "if ";
      ExpressionParser expr(&database);
      for (::ast::ConditionalStatement c : p->conditionalStatements.list) {
	if (c.condition) {
	  ast::ObjectValueContainer expectedType(ast::BOOLEAN);
	  println(parm, command + " (" + expr.toString(c.condition, expectedType) + ") {");
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
      println(parm, "wait(" + p->physical->number->toString() + ");");
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
