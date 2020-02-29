#include "systemc.hpp"
#include "sequential.hpp"
#include "expression/expression.hpp"

namespace generator {

  void SystemC::procedureCallStatement(parameters& parm, ast::ProcedureCallStatement* p) {
    if (p) {
      debug.functionStart("procedureCallStatement");
      parm.addImplementationContents(a_expression.procedureCallStatementToString(parm, p) + ";");
      debug.functionEnd("procedureCallStatement");
    }
  }

  bool SystemC::getObjectName(parameters& parm, std::string& name, ast::ObjectValueContainer& type, ast::ObjectType id, ast::Text* text) {
    debug.functionStart("getObjectName(name = " + name + ", type = " + type.toString() + ", id = " + ast::toString(id));
    bool result = false;
    DatabaseResult object;
    if (parm.findOne(object, name, id)) {
      name = NameConverter::getName(parm, object);
      type = object.object->type;
      result = true;
    } else {
      exceptions.printError("Cound to find definition of " + ast::toString(id) + " with name " + name, text);
      parm.printAllObjects(name);
    }
    debug.functionEnd("getObjectName");
    return result;
  }
  
  bool SystemC::getObjectName(parameters& parm, std::string& name, ast::ObjectType id, ast::Text* text) {
    ast::ObjectValueContainer o;
    return getObjectName(parm, name, o, id, text);
  }
  
  void SystemC::variableAssignment(parameters& parm, ast::VariableAssignment* p) {
    if (p) {
      debug.functionStart("variableAssignment");
      assignment(parm, p->assignment, p->target, ast::ObjectType::VARIABLE, [](DatabaseResult& object, std::string& name_extension) {name_extension = ".read()";});
      debug.functionEnd("variableAssignment");
    }
  }

  void SystemC::signalAssignment(parameters& parm, ast::SignalAssignment* p) {
    if (p) {
      debug.functionStart("signalAssignment");
      signalAssignment(parm, p, [](DatabaseResult& object, std::string& name_extension) {name_extension = ".read()";});
      debug.functionEnd("signalAssignment");
    }
  }

  void SystemC::reportStatement(parameters& parm, ast::ReportStatement* p) {
    if (p) {
      debug.functionStart("reportStatement");
      static ast::ObjectValueContainer enum_type =  ast::ObjectValueContainer(ast::ObjectValue::ENUMERATION);
      static ast::ObjectValueContainer expected_type =
        ast::ObjectValueContainer(ast::ObjectValue::ARRAY, ast::ObjectValueContainer(ast::ObjectValue::INTEGER), enum_type); 
      std::string severity = p->severity->toString(true);
      DatabaseResult object;
      if (parm.findOne(object, severity, ast::ObjectType::ENUM)) {
        std::string name = NameConverter::getName(parm, object);
        parm.addImplementationContents("vhdl::report(" +
				       a_expression.toString(parm, p->message, expected_type, true) + ", " +
				       name + ");");
      } else {
        exceptions.printError("Cound to find severity level " + severity, &p->severity->text);
        parm.printAllObjects(severity);
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
            std::string condition = a_expression.toString(parm, c.condition, expectedType);
            parm.addImplementationContents(command + " (" + condition + ") {");
          } catch (ExpressionParser::ObjectNotFound e) {
            e.print();
          }
	} else {
	  parm.addImplementationContents("} else {");
	}
	command = "} elsif";
        sequentialStatements(parm, c.sequentialStatements);
      }
      parm.addImplementationContents("}");
      debug.functionEnd("ifStatement");
    }
  }

  void SystemC::forLoopStatement(parameters& parm, ast::ForLoopStatement* f) {
    if (f) {
      assert(f->identifier);
      std::string name = f->identifier->toString(true);
      debug.functionStart("forLoopStatement(name = " + name + ")");
      auto callback =
	[&](parameters& parm,
	    std::string& forloop_execution,
	    std::string& variable_instance,
	    std::string& variable_creation)
	{
	  parm.addClassContents(variable_instance);
	  parm.addClassConstructorContents(variable_creation);
	  parm.addImplementationContents(forloop_execution);
	  sequentialStatements(parm, f->sequentialStatements);
	  parm.addImplementationContents("}");
      };
      forLoop(parm, name, f->iteration, callback);
      debug.functionEnd("forLoopStatement");
    }
  }
  
  void SystemC::waitStatement(parameters& parm, ast::WaitStatement* p) {
    if (p) {
      debug.functionStart("waitStatement");
      assert(p->waitText);
      parm.process_contains_wait = true;
      parm.addImplementationContents(getSourceLine(p->waitText));
      parm.addImplementationContents(NameConverter::getTopLevelPrefix(parm) + "wait(vhdl::convert(" + a_expression.physicalToString(parm, p->physical) + "));"); 
      debug.functionEnd("waitStatement");
    }
  }

  void SystemC::returnStatement(parameters& parm, ast::ReturnStatement* r) {
    if (r) {
       debug.functionStart("returnStatement");
       try {
         parm.addImplementationContents("return " + a_expression.toString(parm, r->value, parm.returnType) + ";");
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
