#include "systemc.hpp"
#include "sequential.hpp"
#include "expression/expression.hpp"

namespace generator {

  void SystemC::procedureCallStatement(parameters& parm, ast::ProcedureCallStatement* p, std::list<std::string>& sequential_list) {
    if (p) {
      m_debug.functionStart("procedureCallStatement");
      parm.addTextToList(sequential_list, a_expression.procedureCallStatementToString(parm, p) + ";", __FILE__, __LINE__);
      m_debug.functionEnd("procedureCallStatement");
    }
  }

  bool SystemC::getObjectName(parameters& parm, std::string& name, ast::ObjectValueContainer& type, ast::ObjectType id, ast::Text* text) {
    m_debug.functionStart("getObjectName(name = " + name + ", type = " + type.toString() + ", id = " + ast::toString(id));
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
    m_debug.functionEnd("getObjectName");
    return result;
  }
  
  bool SystemC::getObjectName(parameters& parm, std::string& name, ast::ObjectType id, ast::Text* text) {
    ast::ObjectValueContainer o;
    return getObjectName(parm, name, o, id, text);
  }
  
  void SystemC::variableAssignment(parameters& parm, ast::VariableAssignment* p, std::list<std::string>& sequential_list) {
    if (p) {
      m_debug.functionStart("variableAssignment");
      assignment(parm, p->assignment, p->target, ast::ObjectType::VARIABLE, sequential_list,
		 [](DatabaseResult& object, std::string& name_extension) {name_extension = ".read()";});
      m_debug.functionEnd("variableAssignment");
    }
  }

  bool SystemC::lookupSeverity(parameters& parm, ast::ReportStatement* p, std::string& severity, std::list<std::string>& sequential_list) {
    bool found;
    static ast::ObjectValueContainer enum_type =  ast::ObjectValueContainer(ast::ObjectValue::ENUMERATION);
    static ast::ObjectValueContainer expected_type =
      ast::ObjectValueContainer(ast::ObjectValue::ARRAY, ast::ObjectValueContainer(ast::ObjectValue::INTEGER), enum_type); 
    std::string message = a_expression.toString(parm, p->message, expected_type, true);
    if (m_convert_to_systemc) {
      static std::unordered_map<std::string, std::string> severity_lookup({{"NOTE", "INFO"}, {"WARNING", "WARNING"}, {"ERROR", "ERROR"}, {"FAILURE", "FATAL"}});
      auto lookup = severity_lookup.find(severity);
      found = (lookup != severity_lookup.end());
      if (found) {
        parm.addTextToList(sequential_list, "SC_REPORT_" + lookup->second + "(\"VHDL report\", std::string(" +
			   message + ").c_str());", __FILE__, __LINE__);
      }
    } else {
      DatabaseResult object;
      found = parm.findOne(object, severity, ast::ObjectType::ENUM);
      if (found) {
        std::string name = NameConverter::getName(parm, object);
        parm.addTextToList(sequential_list, "vhdl::report(" + message + ", " +
			   name + ", __FILE__, __LINE__);", __FILE__, __LINE__);
      }
    }
    return found;
  }
  
  void SystemC::reportStatement(parameters& parm, ast::ReportStatement* p, std::list<std::string>& sequential_list) {
    if (p) {
      m_debug.functionStart("reportStatement");
      std::string severity = p->severity->toString(true);
      if (!lookupSeverity(parm, p, severity, sequential_list)) {
        exceptions.printError("Cound to find severity level " + severity + ". Must be one of NOTE, WARNING, ERROR or FAILURE", &p->severity->text);
        parm.printAllObjects(severity);
      }
      m_debug.functionEnd("reportStatement");
    }
  }

  void SystemC::waitStatement(parameters& parm, ast::WaitStatement* p, std::list<std::string>& sequential_list) {
    if (p) {
      m_debug.functionStart("waitStatement");
      assert(p->waitText);
      parm.process_contains_wait = true;
      parm.addTextToList(sequential_list, getSourceLine(p->waitText), __FILE__, __LINE__);
      parm.addTextToList(sequential_list, NameConverter::getTopLevelPrefix(parm) + "wait(vhdl::convert(" + a_expression.physicalToString(parm, p->physical) + "));", __FILE__, __LINE__); 
      m_debug.functionEnd("waitStatement");
    }
  }

  void SystemC::returnStatement(parameters& parm, ast::ReturnStatement* r, std::list<std::string>& sequential_list) {
    if (r) {
       m_debug.functionStart("returnStatement");
       try {
         parm.addTextToList(sequential_list, "return " + a_expression.toString(parm, r->value, parm.returnType) + ";", __FILE__, __LINE__);
       } catch (ExpressionParser::ObjectNotFound e) {
         e.print();
       }
       m_debug.functionEnd("returnStatement");
    }
  }

}
