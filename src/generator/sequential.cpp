#include "systemc.hpp"
#include "sequential.hpp"
#include "expression/expression.hpp"

namespace generator {

  void SystemC::procedureCallStatement(parameters& parm, ast::ProcedureCallStatement* p) {
    if (p) {
      if (parm.isArea(parameters::IMPLEMENTATION)) {
	ExpressionParser expr(&database);
	parm.println(expr.procedureCallStatementToString(p) + ";");
      }
    }
  }
  
  void SystemC::variableAssignment(parameters& parm, ast::VariableAssignment* p) {
    if (p) {
      if (parm.isArea(parameters::IMPLEMENTATION)) {
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
  }

  void SystemC::signalAssignment(parameters& parm, ast::SignalAssignment* p) {
    if (parm.isArea(parameters::IMPLEMENTATION)) {
      signalAssignment(parm, p,  [](DatabaseResult& object) {});
    }
  }

  void SystemC::reportStatement(parameters& parm, ast::ReportStatement* p) {
    if (p) {
      if (parm.isArea(parameters::IMPLEMENTATION)) {
	ExpressionParser expr(&database);
	static ast::ObjectValueContainer expectedType = database.getType("STRING", "STANDARD", "STD");
	std::string severity = p->severity->toString(true);
	if (database.globalName(severity, ast::ENUM)) {
	  parm.println("report(" + expr.toString(p->message, expectedType) + ", " +
		       severity + ");");
	} else {
	  exceptions.printError("Cound to find severity level " + severity, &p->severity->text);
	  database.printAllObjects(severity);
	}
      }
    }
  }

  void SystemC::ifStatement(parameters& parm, ast::IfStatement* p) {
    if (p) {
      std::string command = "if ";
      ExpressionParser expr(&database);
      for (::ast::ConditionalStatement c : p->conditionalStatements.list) {
	if (c.condition) {
          static ast::ObjectValueContainer expectedType = database.getType("BOOLEAN", "STANDARD", "STD");
	  parm.println(parameters::IMPLEMENTATION, command + " (" + expr.toString(c.condition, expectedType) + ") {");
	} else {
	  parm.println(parameters::IMPLEMENTATION, "} else {");
	}
	command = "} elsif";
        parm.incIndent();
        sequentialStatements(parm, c.sequentialStatements);
        parm.decIndent();
      }
      parm.println(parameters::IMPLEMENTATION, "}");
    }
  }

  void SystemC::forLoopStatement(parameters& parm, ast::ForLoopStatement* f) {
    if (f) {
      assert(f->identifier);
      std::string name = f->identifier->toString(true);
      if (parm.isArea(parameters::IMPLEMENTATION)) {
	descendHierarchy(parm);
      }
      auto callback = [&](parameters& parm) {
	sequentialStatements(parm, f->sequentialStatements);
      };
      forLoop(parm, name, f->range, callback);
      if (parm.isArea(parameters::IMPLEMENTATION)) {
	ascendHierarchy(parm);
      }
    }
  }
  
  void SystemC::waitStatement(parameters& parm, ast::WaitStatement* p) {
    if (p) {
      assert(p->waitText);
      std::string label = "line" + std::to_string(p->waitText->getLine());
      if (parm.isArea(parameters::INITIALIZATION)) {
	parm.println("case " + std::to_string(parm.index++) + ": goto " + label + ";");
      }
      if (parm.isArea(parameters::IMPLEMENTATION)) {
	ExpressionParser expr(&database);
	std::string now = database.globalName("NOW") + "()";
	parm.println("waitTime = " + expr.physicalToString(p->physical) + ";"); 
	parm.println("waitUntil = " + now + " + waitTime;");
	parm.println(0, label + ":");
	printSourceLine(parm, p->waitText);
	parm.println("if (wait(waitUntil)) {waitIndex++;} else {return;};");
      }
    }
  }

  void SystemC::returnStatement(parameters& parm, ast::ReturnStatement* r) {
    if (r) {
      if (parm.isArea(parameters::IMPLEMENTATION)) {
	ExpressionParser expr(&database);
	parm.println("return " + expr.toString(r->value, parm.returnType) + ";");
      }
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
