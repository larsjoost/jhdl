#include "systemc.hpp"
#include "expression/expression.hpp"

namespace generator {

  template<typename Func>
  void SystemC::forLoop(parameters& parm, std::string& name, ast::IterationScheme* iteration, Func callback) {
    m_debug.functionStart("forLoop(name = " + name + ")", false, __FILE__, __LINE__);
    std::string typeName;
    ast::ObjectValueContainer type;
    std::string variable_instance;
    std::string variable_creation;
    if (iteration->range) {
      ast::RangeType* r = iteration->range;
      typeName = name + "_type";
      type = printRangeType(parm, typeName, r);
      std::string factory_name = NameConverter::getFactoryInstanceName(typeName) + "->create()";
      variable_creation = name + ".constrain(" + factory_name + ");"; 
    } else if (iteration->identifier) {
      DatabaseResult object;
      if (parm.findOne(object, iteration->identifier)) {  
        typeName = NameConverter::getName(parm, object);
        if (iteration->range_attribute) {
          if (object.object->type.GetValue() == ast::ObjectValue::ARRAY) { 
            type = object.object->type.GetArgument();
          } else {
            exceptions.printError(name + " must be array type to use range attribute",
				  &iteration->identifier->text);
          }
        } else {
          type = object.object->type;
        }
      } 
    }
    variable_instance = typeName + " " + name + ";"; 
    parm.addObjectValueContainer(ast::ObjectType::VARIABLE, name, type);
    std::string forloop_execution = "for (" +
      name + " = " + name + ".LEFT(); " +
      name + " <= " + name + ".RIGHT(); " +
      name + " = " + name + ".RIGHTOF()) {";
    callback(parm, forloop_execution, variable_instance, variable_creation, type);
    m_debug.functionEnd("forLoop");
  }  

  template<typename Func>
  void SystemC::assignment(parameters& parm, ast::Assignment* p, ast::BasicIdentifier* target, ast::ObjectType object_type,
			   std::list<std::string>& sequential_list, Func sensitivity_list_callback, bool signal_assignment) {
    m_debug.functionStart("assignment", false, __FILE__, __LINE__);
    std::string command = "if";
    std::string noConditionCommand = "";
    std::string noConditionDelimiter = "";

    try {
      DatabaseResult database_result;
      std::string name = a_expression.basicIdentifierToString(parm, target, database_result);
      if (signal_assignment) {
	std::string hierarchy = parm.hierarchyToString();
	int current_index = database_result.object->getSignalAssignmentIndex(hierarchy, &target->getText());
	name += "[" + std::to_string(current_index) + "]";
      }
      parm.addTextToList(sequential_list, getSourceLine(target), __FILE__, __LINE__);
      ast::ReturnTypes return_types;
      a_expression.basicIdentifierReturnTypes(parm, target, return_types);
      ast::ObjectTypes expectedType;
      ast::ObjectValueContainer found_type;
      if (a_expression.uniqueReturnType(parm, return_types, found_type, &target->getText())) {
	ExpectedType expected_type(found_type);
	for (ast::AssignmentCondition s : p->assignment_conditions.list) {
	  if (s.condition) {
	    static ExpectedType e(ast::ObjectValue::BOOLEAN);
	    parm.addTextToList(sequential_list, command + " (" + a_expression.assignmentString(parm, s.condition, e, sensitivity_list_callback) + ") {", __FILE__, __LINE__);
	    command = "else if";
	    noConditionCommand = "else {";
	    noConditionDelimiter = "}";
	  } else {
	    parm.addTextToList(sequential_list, noConditionCommand, __FILE__, __LINE__);
	  }
	  parm.addTextToList(sequential_list, name + " = " + a_expression.assignmentString(parm, s.expression, expected_type, sensitivity_list_callback, name) + ";", __FILE__, __LINE__, false, &target->getText());
	  if (s.condition) {
	    parm.addTextToList(sequential_list, "}", __FILE__, __LINE__);
	  } else {
	    parm.addTextToList(sequential_list, noConditionDelimiter, __FILE__, __LINE__);
	  }
	}
      } else {
	exceptions.printError("Could not find definition of " + ast::toString(object_type) + " \"" + name + "\"", &target->getText());
	parm.printAllObjects(name);
      }
    } catch (ExpressionParser::ObjectNotFound& e) {
      e.print();
    }
    m_debug.functionEnd("assignment");
  }
  
  template <typename Func>
  void SystemC::signalAssignment(parameters& parm, ast::SignalAssignment* p, std::list<std::string>& sequential_list, Func sensitivity_list_callback) {
    if (p) {
      m_debug.functionStart("signalAssignment", false, __FILE__, __LINE__);
      assignment(parm, p->assignment, p->target, ast::ObjectType::SIGNAL, sequential_list, sensitivity_list_callback, true);
      m_debug.functionEnd("signalAssignment");
    }
  }

  template <typename Func>
  void SystemC::ifStatement(parameters& parm, ast::IfStatement* p, std::list<std::string>& sequential_list, Func sensitivity_list_callback) {
    if (p) {
      m_debug.functionStart("ifStatement", false, __FILE__, __LINE__);
      std::string command = "if ";
      for (::ast::ConditionalStatement c : p->conditionalStatements.list) {
	if (c.condition) {
          ExpectedType expectedType(ast::ObjectValue::BOOLEAN);
          try {
            std::string condition = a_expression.toString(parm, c.condition, expectedType);
            parm.addTextToList(sequential_list, command + " (" + condition + ") {", __FILE__, __LINE__);
          } catch (ExpressionParser::ObjectNotFound& e) {
            e.print();
          }
	} else {
	  parm.addTextToList(sequential_list, "} else {", __FILE__, __LINE__);
	}
	command = "} else if";
        sequentialStatements(parm, c.sequentialStatements, sequential_list, sensitivity_list_callback);
      }
      parm.addTextToList(sequential_list, "}", __FILE__, __LINE__);
      m_debug.functionEnd("ifStatement");
    }
  }

  template <typename Func>
  void SystemC::forLoopStatement(parameters& parm, ast::ForLoopStatement* f, std::list<std::string>& sequential_list, Func sensitivity_list_callback) {
    if (f) {
      assert(f->identifier);
      std::string name = f->identifier->toString(true);
      m_debug.functionStart("forLoopStatement(name = " + name + ")", false, __FILE__, __LINE__, &f->identifier->text);
      auto callback =
	[&](parameters& parm,
	    std::string& forloop_execution,
	    std::string& variable_instance,
	    std::string& variable_creation,
	    ast::ObjectValueContainer variable_type)
	{
	  parm.addClassContents(variable_instance, __FILE__, __LINE__);
	  parm.addClassConstructorContents(variable_creation, __FILE__, __LINE__);
	  parm.addTextToList(sequential_list, forloop_execution, __FILE__, __LINE__);
	  sequentialStatements(parm, f->sequentialStatements, sequential_list, sensitivity_list_callback);
	  parm.addTextToList(sequential_list, "}", __FILE__, __LINE__);
      };
      forLoop(parm, name, f->iteration, callback);
      m_debug.functionEnd("forLoopStatement");
    }
  }
  

  template <typename Func>
  void SystemC::sequentialStatements(parameters& parm, ast::List<ast::SequentialStatement>& l,
				     std::list<std::string>& sequential_list, Func sensitivity_list_callback) {
    m_debug.functionStart("sequentialStatements");
    for (ast::SequentialStatement s : l.list) {
      procedureCallStatement(parm, s.procedureCallStatement, sequential_list);
      variableAssignment(parm, s.variableAssignment, sequential_list);
      signalAssignment(parm, s.signalAssignment, sequential_list, sensitivity_list_callback);
      reportStatement(parm, s.reportStatement, sequential_list);
      ifStatement(parm, s.ifStatement, sequential_list, sensitivity_list_callback);
      forLoopStatement(parm, s.forLoopStatement, sequential_list, sensitivity_list_callback);
      waitStatement(parm, s.waitStatement, sequential_list);
      returnStatement(parm, s.returnStatement, sequential_list);
    }
    m_debug.functionEnd("sequentialStatements");
  }


}
