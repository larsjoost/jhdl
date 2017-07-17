#include "systemc.hpp"
#include "sequential.hpp"
#include "expression/expression.hpp"

namespace generator {

  void SystemC::procedureCallStatement(parameters& parm, ast::ProcedureCallStatement* p) {
    if (p) {
      ExpressionParser expr(&database);
      parm.println(expr.procedureCallStatementToString(p) + ";");
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
        name = object.getName(true);
	parm.println(name + " = " + expr.toString(p->expression, object.object->type) + ";");
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
      static ast::ObjectValueContainer expectedType = database.getType("STRING", "STANDARD", "STD");
      parm.println("report(" + expr.toString(p->message, expectedType) + ", " +
              p->severity->toString() + ");");
    }
  }

    void SystemC::ifStatement(parameters& parm, ast::IfStatement* p) {
    if (p) {
      std::string command = "if ";
      ExpressionParser expr(&database);
      for (::ast::ConditionalStatement c : p->conditionalStatements.list) {
	if (c.condition) {
          static ast::ObjectValueContainer expectedType = database.getType("BOOLEAN", "STANDARD", "STD");
	  parm.println(command + " (" + expr.toString(c.condition, expectedType) + ") {");
	} else {
	  parm.println("} else {");
	}
	command = "} elsif";
        parm.incIndent();
        sequentialStatements(parm, c.sequentialStatements);
        parm.decIndent();
      }
      parm.println("}");
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
      parm.println("wait(" + p->physical->number->toString() + ");");
    }
  }


  void SystemC::returnStatement(parameters& parm, ast::ReturnStatement* r) {
    if (r) {
      ExpressionParser expr(&database);
      parm.println("return " + expr.toString(r->value, parm.returnType) + ";");
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
