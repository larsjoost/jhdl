#include "systemc.hpp"
#include "sequential.hpp"
#include "expression/expression.hpp"

namespace generator {

  void SystemC::procedureCallStatement(parameters& parm, ast::ProcedureCallStatement* p, std::list<std::string>& sequential_list) {
    if (p) {
      debug.functionStart("procedureCallStatement");
      parm.addTextToList(sequential_list, a_expression.procedureCallStatementToString(parm, p) + ";", __FILE__, __LINE__);
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
  
  void SystemC::variableAssignment(parameters& parm, ast::VariableAssignment* p, std::list<std::string>& sequential_list) {
    if (p) {
      debug.functionStart("variableAssignment");
      assignment(parm, p->assignment, p->target, ast::ObjectType::VARIABLE, sequential_list,
		 [](DatabaseResult& object, std::string& name_extension) {name_extension = ".read()";});
      debug.functionEnd("variableAssignment");
    }
  }

  void SystemC::reportStatement(parameters& parm, ast::ReportStatement* p, std::list<std::string>& sequential_list) {
    if (p) {
      debug.functionStart("reportStatement");
      static ast::ObjectValueContainer enum_type =  ast::ObjectValueContainer(ast::ObjectValue::ENUMERATION);
      static ast::ObjectValueContainer expected_type =
        ast::ObjectValueContainer(ast::ObjectValue::ARRAY, ast::ObjectValueContainer(ast::ObjectValue::INTEGER), enum_type); 
      std::string severity = p->severity->toString(true);
      DatabaseResult object;
      if (parm.findOne(object, severity, ast::ObjectType::ENUM)) {
        std::string name = NameConverter::getName(parm, object);
        parm.addTextToList(sequential_list, "vhdl::report(" +
			   a_expression.toString(parm, p->message, expected_type, true) + ", " +
			   name + ");", __FILE__, __LINE__);
      } else {
        exceptions.printError("Cound to find severity level " + severity, &p->severity->text);
        parm.printAllObjects(severity);
      }
      debug.functionEnd("reportStatement");
    }
  }

  void SystemC::waitStatement(parameters& parm, ast::WaitStatement* p, std::list<std::string>& sequential_list) {
    if (p) {
      debug.functionStart("waitStatement");
      assert(p->waitText);
      parm.process_contains_wait = true;
      parm.addTextToList(sequential_list, getSourceLine(p->waitText), __FILE__, __LINE__);
      parm.addTextToList(sequential_list, NameConverter::getTopLevelPrefix(parm) + "wait(vhdl::convert(" + a_expression.physicalToString(parm, p->physical) + "));", __FILE__, __LINE__); 
      debug.functionEnd("waitStatement");
    }
  }

  void SystemC::returnStatement(parameters& parm, ast::ReturnStatement* r, std::list<std::string>& sequential_list) {
    if (r) {
       debug.functionStart("returnStatement");
       try {
         parm.addTextToList(sequential_list, "return " + a_expression.toString(parm, r->value, parm.returnType) + ";", __FILE__, __LINE__);
       } catch (ExpressionParser::ObjectNotFound e) {
         e.print();
       }
       debug.functionEnd("returnStatement");
    }
  }

}
