#include "systemc.hpp"
#include "sequential.hpp"
#include "expression/expression.hpp"

namespace generator {

  void SystemC::procedureCallStatement(parameters& parm, ast::ProcedureCallStatement* p) {
    if (p) {
      debug.functionStart("procedureCallStatement");
      parm.println(parameters::Area::IMPLEMENTATION, a_expression.procedureCallStatementToString(p) + ";");
      debug.functionEnd("procedureCallStatement");
    }
  }

  bool SystemC::getObjectName(std::string& name, ast::ObjectValueContainer& type, ast::ObjectType id, ast::Text* text) {
    bool result = false;
    DatabaseResult object;
    if (a_database.findOne(object, name, id)) {
      name = a_name_converter.getName(object, true);
      type = object.object->type;
      result = true;
    } else {
      exceptions.printError("Cound to find definition of " + ast::toString(id) + " with name " + name, text);
      a_database.printAllObjects(name);
    }
    return result;
  }
  
  bool SystemC::getObjectName(std::string& name, ast::ObjectType id, ast::Text* text) {
    ast::ObjectValueContainer o;
    return getObjectName(name, o, id, text);
  }
  
  void SystemC::variableAssignment(parameters& parm, ast::VariableAssignment* p) {
    if (p) {
      printSourceLine(parm, p->identifier);
      std::string name = p->identifier->toString(true);
      ast::ObjectValueContainer type;
      if (getObjectName(name, type, ast::ObjectType::VARIABLE, &p->identifier->text)) {
        try {
          parm.println(parameters::Area::IMPLEMENTATION, name + " = " + a_expression.toString(p->expression, type) + ";");
        } catch (ExpressionParser::ObjectNotFound e) {
          e.print();
        }
      }
    }
  }

  void SystemC::signalAssignment(parameters& parm, ast::SignalAssignment* p) {
    signalAssignment(parm, p,  [](DatabaseResult& object) {});
  }

  void SystemC::reportStatement(parameters& parm, ast::ReportStatement* p) {
    if (p) {
      debug.functionStart("reportStatement");
      static ast::ObjectValueContainer expectedType = a_database.getType("STRING", "STANDARD", "STD");
      std::string severity = p->severity->toString(true);
      DatabaseResult object;
      if (a_database.findOne(object, severity, ast::ObjectType::ENUM)) {
        std::string name = a_name_converter.getName(object, true);
        parm.println(parameters::Area::IMPLEMENTATION,
                     "report(" + a_expression.toString(p->message, expectedType) + ", " +
                     name + ");");
      } else {
        exceptions.printError("Cound to find severity level " + severity, &p->severity->text);
        a_database.printAllObjects(severity);
      }
      debug.functionEnd("reportStatement");
    }
  }

  void SystemC::ifStatement(parameters& parm, ast::IfStatement* p) {
    if (p) {
      std::string command = "if ";
      for (::ast::ConditionalStatement c : p->conditionalStatements.list) {
	if (c.condition) {
          static ast::ObjectValueContainer expectedType(ast::ObjectValue::BOOLEAN);
	  parm.println(parameters::Area::IMPLEMENTATION, command + " (" + a_expression.toString(c.condition, expectedType) + ") {");
	} else {
	  parm.println(parameters::Area::IMPLEMENTATION, "} else {");
	}
	command = "} elsif";
        parm.incIndent();
        sequentialStatements(parm, c.sequentialStatements);
        parm.decIndent();
      }
      parm.println(parameters::Area::IMPLEMENTATION, "}");
    }
  }

  void SystemC::forLoopStatement(parameters& parm, ast::ForLoopStatement* f) {
    if (f) {
      assert(f->identifier);
      std::string name = f->identifier->toString(true);
      auto callback = [&](parameters& parm) {
	sequentialStatements(parm, f->sequentialStatements);
      };
      forLoop(parm, name, f->iteration, callback);
    }
  }
  
  void SystemC::waitStatement(parameters& parm, ast::WaitStatement* p) {
    if (p) {
      assert(p->waitText);
      std::string index = std::to_string(parm.index);
      std::string label = "line" + std::to_string(p->waitText->getLine());
      parm.println(parameters::Area::INITIALIZATION, "case " + index + ": goto " + label + ";");
      std::string now = a_database.globalName("NOW") + "()";
      parm.println(parameters::Area::IMPLEMENTATION, "w.waitFor(" + a_expression.physicalToString(p->physical) + ");"); 
      printSourceLine(parm, p->waitText);
      parm.println(parameters::Area::IMPLEMENTATION, label + ": if (w.done()) {w.index = 0;} else {w.index = " + index + "; return;};");
      parm.index++;
    }
  }

  void SystemC::returnStatement(parameters& parm, ast::ReturnStatement* r) {
    if (r) {
       debug.functionStart("returnStatement");
       try {
         parm.println(parameters::Area::IMPLEMENTATION, "return " + a_expression.toString(r->value, parm.returnType) + ";");
       } catch (ExpressionParser::ObjectNotFound e) {
         e.print();
       }
       debug.functionEnd("returnStatement");
    }
  }

  void SystemC::sequentialStatements(parameters& parm, ast::List<ast::SequentialStatement>& l) {
    debug.functionStart("sequentialStatements");
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
    debug.functionEnd("sequentialStatements");
  }

}
