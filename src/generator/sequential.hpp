#include "systemc.hpp"
#include "expression/expression.hpp"

namespace generator {

  template<typename Func>
  void SystemC::forLoop(parameters& parm, std::string& name, ast::IterationScheme* iteration, Func callback) {
    debug.functionStart("forLoop(name = " + name + ")");
    std::string typeName;
    ast::ObjectValueContainer type;
    std::string variable_instance;
    std::string variable_creation;
    if (iteration->range) {
      ast::RangeType* r = iteration->range;
      typeName = name + "_type";
      type = ast::ObjectValueContainer(ast::ObjectValue::INTEGER);
      printRangeType(parm, typeName, r);
      variable_instance = typeName + " " + name + ";"; 
      std::string factory_name = "factory_" + typeName + ".create()";
      variable_creation = name + ".construct(" + factory_name + ");"; 
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
    parm.addObject(ast::ObjectType::VARIABLE, name, type);
    std::string forloop_execution = "for (" +
      name + " = " + name + ".LEFT(); " +
      name + " <= " + name + ".RIGHT(); " +
      name + " = " + name + ".RIGHTOF()) {";
    callback(parm, forloop_execution, variable_instance, variable_creation);
    debug.functionEnd("forLoop");
  }  

  template<typename Func>
  void SystemC::assignment(parameters& parm, ast::Assignment* p, ast::BasicIdentifier* target, ast::ObjectType object_type,
			   std::list<std::string>& sequential_list, Func sensitivity_list_callback) {
    debug.functionStart("assignment");
    std::string command = "if";
    std::string noConditionCommand = "";
    std::string noConditionDelimiter = "";

    try {
      std::string name = a_expression.basicIdentifierToString(parm, target);
      parm.addTextToList(sequential_list, getSourceLine(target), __FILE__, __LINE__);
      ast::ReturnTypes return_types;
      a_expression.basicIdentifierReturnTypes(parm, target, return_types);
      ast::ObjectValueContainer expectedType;
      if (a_expression.uniqueReturnType(parm, return_types, expectedType, &target->getText())) {
	for (ast::AssignmentCondition s : p->assignment_conditions.list) {
	  if (s.condition) {
	    static ast::ObjectValueContainer expectedValue(ast::ObjectValue::BOOLEAN);
	    parm.addTextToList(sequential_list, command + " (" + a_expression.toString(parm, s.condition, expectedValue, sensitivity_list_callback) + ") {", __FILE__, __LINE__);
	    command = "else if";
	    noConditionCommand = "else {";
	    noConditionDelimiter = "}";
	  } else {
	    parm.addTextToList(sequential_list, noConditionCommand, __FILE__, __LINE__);
	  }
	  parm.addTextToList(sequential_list, name + " = " + a_expression.toString(parm, s.expression, expectedType, sensitivity_list_callback) + ";", __FILE__, __LINE__);
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
    } catch (ExpressionParser::ObjectNotFound e) {
      e.print();
    }
    debug.functionEnd("assignment");
  }
  
  template <typename Func>
  void SystemC::signalAssignment(parameters& parm, ast::SignalAssignment* p, std::list<std::string>& sequential_list, Func sensitivity_list_callback) {
    if (p) {
      debug.functionStart("signalAssignment");
      assignment(parm, p->assignment, p->target, ast::ObjectType::SIGNAL, sequential_list, sensitivity_list_callback);
      debug.functionEnd("signalAssignment");
    }
  }

  template <typename Func>
  void SystemC::ifStatement(parameters& parm, ast::IfStatement* p, std::list<std::string>& sequential_list, Func sensitivity_list_callback) {
    if (p) {
      debug.functionStart("ifStatement");
      std::string command = "if ";
      for (::ast::ConditionalStatement c : p->conditionalStatements.list) {
	if (c.condition) {
          static ast::ObjectValueContainer expectedType(ast::ObjectValue::BOOLEAN);
          try {
            std::string condition = a_expression.toString(parm, c.condition, expectedType);
            parm.addTextToList(sequential_list, command + " (" + condition + ") {", __FILE__, __LINE__);
          } catch (ExpressionParser::ObjectNotFound e) {
            e.print();
          }
	} else {
	  parm.addTextToList(sequential_list, "} else {", __FILE__, __LINE__);
	}
	command = "} elsif";
        sequentialStatements(parm, c.sequentialStatements, sequential_list, sensitivity_list_callback);
      }
      parm.addTextToList(sequential_list, "}", __FILE__, __LINE__);
      debug.functionEnd("ifStatement");
    }
  }

  template <typename Func>
  void SystemC::forLoopStatement(parameters& parm, ast::ForLoopStatement* f, std::list<std::string>& sequential_list, Func sensitivity_list_callback) {
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
	  parm.addTextToList(sequential_list, forloop_execution, __FILE__, __LINE__);
	  sequentialStatements(parm, f->sequentialStatements, sequential_list, sensitivity_list_callback);
	  parm.addTextToList(sequential_list, "}", __FILE__, __LINE__);
      };
      forLoop(parm, name, f->iteration, callback);
      debug.functionEnd("forLoopStatement");
    }
  }
  

  template <typename Func>
  void SystemC::sequentialStatements(parameters& parm, ast::List<ast::SequentialStatement>& l,
				     std::list<std::string>& sequential_list, Func sensitivity_list_callback) {
    debug.functionStart("sequentialStatements");
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
    debug.functionEnd("sequentialStatements");
  }


}
