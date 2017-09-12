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
    debug.functionStart("getObjectName(name = " + name + ", type = " + type.toString() + ", id = " + ast::toString(id));
    bool result = false;
    DatabaseResult object;
    if (a_database.findOne(object, name, id)) {
      name = a_name_converter.GetName(object);
      type = object.object->type;
      result = true;
    } else {
      exceptions.printError("Cound to find definition of " + ast::toString(id) + " with name " + name, text);
      a_database.printAllObjects(name);
    }
    debug.functionEnd("getObjectName");
    return result;
  }
  
  bool SystemC::getObjectName(std::string& name, ast::ObjectType id, ast::Text* text) {
    ast::ObjectValueContainer o;
    return getObjectName(name, o, id, text);
  }
  
  void SystemC::variableAssignment(parameters& parm, ast::VariableAssignment* p) {
    if (p) {
      debug.functionStart("variableAssignment");
      printSourceLine(parm, p->target->identifier);
      std::string name = p->target->identifier->toString(true);
      DatabaseResult object;
      if (a_database.findOne(object, name, ast::ObjectType::VARIABLE)) {  
        ast::ObjectValueContainer expectedType = object.object->type;
        std::string name = a_name_converter.GetName(object);
        try {
          parm.println(parameters::Area::IMPLEMENTATION, name + " = " + a_expression.toString(p->expression, expectedType) + ";");
        } catch (ExpressionParser::ObjectNotFound e) {
          e.print();
        }
      }
      debug.functionEnd("variableAssignment");
    }
  }

  void SystemC::signalAssignment(parameters& parm, ast::SignalAssignment* p) {
    signalAssignment(parm, p,  [](DatabaseResult& object) {});
  }

  void SystemC::reportStatement(parameters& parm, ast::ReportStatement* p) {
    if (p) {
      debug.functionStart("reportStatement");
      static ast::ObjectValueContainer enum_type =  ast::ObjectValueContainer(ast::ObjectValue::ENUMERATION);
      static ast::ObjectValueContainer expected_type =
        ast::ObjectValueContainer(ast::ObjectValue::ARRAY, ast::ObjectValueContainer(ast::ObjectValue::INTEGER), enum_type); 
      std::string severity = p->severity->toString(true);
      DatabaseResult object;
      if (a_database.findOne(object, severity, ast::ObjectType::ENUM)) {
        std::string name = a_name_converter.GetName(object);
        parm.println(parameters::Area::IMPLEMENTATION,
                     "report(" + a_expression.toString(p->message, expected_type) + ", " +
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
      debug.functionStart("ifStatement");
      std::string command = "if ";
      for (::ast::ConditionalStatement c : p->conditionalStatements.list) {
	if (c.condition) {
          static ast::ObjectValueContainer expectedType(ast::ObjectValue::BOOLEAN);
          try {
            std::string condition = a_expression.toString(c.condition, expectedType);
            parm.println(parameters::Area::IMPLEMENTATION, command + " (" + condition + ") {");
          } catch (ExpressionParser::ObjectNotFound e) {
            e.print();
          }
	} else {
	  parm.println(parameters::Area::IMPLEMENTATION, "} else {");
	}
	command = "} elsif";
        sequentialStatements(parm, c.sequentialStatements);
      }
      parm.println(parameters::Area::IMPLEMENTATION, "}");
      debug.functionEnd("ifStatement");
    }
  }

  void SystemC::forLoopStatement(parameters& parm, ast::ForLoopStatement* f) {
    if (f) {
      debug.functionStart("forLoopStatement");
      assert(f->identifier);
      std::string name = f->identifier->toString(true);
      auto callback = [&](parameters& parm) {
	sequentialStatements(parm, f->sequentialStatements);
      };
      forLoop(parm, name, f->iteration, callback);
      debug.functionEnd("forLoopStatement");
    }
  }
  
  void SystemC::waitStatement(parameters& parm, ast::WaitStatement* p) {
    if (p) {
      debug.functionStart("waitStatement");
      assert(p->waitText);
      std::string index = std::to_string(parm.index);
      std::string label = "line" + std::to_string(p->waitText->getLine());
      parm.println(parameters::Area::INITIALIZATION, "case " + index + ": goto " + label + ";");
      std::string now;
      std::string name = "NOW";
      DatabaseResult object;
      if (a_database.findOne(object, name)) {
        now = a_name_converter.GetName(object) + "()";
      } else {
        exceptions.printError("Unable to find " + name);
      }
      parm.println(parameters::Area::IMPLEMENTATION, "w.waitFor(" + a_expression.physicalToString(p->physical) + ");"); 
      printSourceLine(parm, p->waitText);
      parm.println(parameters::Area::IMPLEMENTATION, label + ": if (w.done()) {w.index = 0;} else {w.index = " + index + "; return;};");
      parm.index++;
      debug.functionEnd("waitStatement");
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
