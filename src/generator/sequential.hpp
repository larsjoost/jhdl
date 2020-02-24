#include "systemc.hpp"
#include "expression/expression.hpp"

namespace generator {

  template<typename Func>
  void SystemC::forLoop(parameters& parm, std::string& name, ast::IterationScheme* iteration, Func callback, bool constructor) {
    debug.functionStart("forLoop");
    std::string typeName;
    ast::ObjectValueContainer type;
    if (iteration->range) {
      ast::RangeType* r = iteration->range;
      typeName = name + "_type";
      type = ast::ObjectValueContainer(ast::ObjectValue::INTEGER);
      printRangeType(parm, typeName, r);
      parm.addClassContents(typeName + " " + name + ";");
      std::string factory_name = "factory_" + typeName + ".create()";
      parm.addClassConstructorContents(name + ".construct(" + factory_name + ");");
    } else if (iteration->identifier) {
      DatabaseResult object;
      if (parm.findOne(object, iteration->identifier)) {  
        typeName = NameConverter::getName(parm, object);
        if (iteration->range_attribute) {
          if (object.object->type.GetValue() == ast::ObjectValue::ARRAY) { 
            type = object.object->type.GetArgument();
          } else {
            exceptions.printError(name + " must be array type to use range attribute", &iteration->identifier->text);
          }
        } else {
          type = object.object->type;
        }
      } 
    }
    parm.addObject(ast::ObjectType::VARIABLE, name, type);
    std::string t = "for (" +
      name + " = " + name + ".LEFT(); " +
      name + " <= " + name + ".RIGHT(); " +
      name + " = " + name + ".RIGHTOF()) {";
    if (constructor) {
      parm.addClassConstructorContents(t);
    } else {
      parm.addImplementationContents(t);
    }
    callback(parm);
    if (constructor) {
      parm.addClassConstructorContents("}");
    } else {
      parm.addImplementationContents("}");
    }
    debug.functionEnd("forLoop");
  }  

  template<typename Func>
  void SystemC::assignment(parameters& parm, ast::Assignment* p, ast::BasicIdentifier* target, ast::ObjectType object_type, Func callback) {
    debug.functionStart("assignment");
    std::string command = "if";
    std::string noConditionCommand = "";
    std::string noConditionDelimiter = "";

    try {
      std::string name = a_expression.basicIdentifierToString(parm, target);
      parm.addImplementationContents(getSourceLine(target));
      ast::ReturnTypes return_types;
      a_expression.basicIdentifierReturnTypes(parm, target, return_types);
      ast::ObjectValueContainer expectedType;
      if (a_expression.uniqueReturnType(parm, return_types, expectedType, &target->getText())) {
	for (ast::AssignmentCondition s : p->assignment_conditions.list) {
	  if (s.condition) {
	    static ast::ObjectValueContainer expectedValue(ast::ObjectValue::BOOLEAN);
	    parm.addImplementationContents(command + " (" + a_expression.toString(parm, s.condition, expectedValue, callback) + ") {");
	    command = "else if";
	    noConditionCommand = "else {";
	    noConditionDelimiter = "}";
	  } else {
	    parm.addImplementationContents(noConditionCommand);
	  }
	  parm.addImplementationContents(name + " = " + a_expression.toString(parm, s.expression, expectedType, callback) + ";");
	  if (s.condition) {
	    parm.addImplementationContents("}");
	  } else {
	    parm.addImplementationContents(noConditionDelimiter);
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
  void SystemC::signalAssignment(parameters& parm, ast::SignalAssignment* p, Func callback) {
    if (p) {
      debug.functionStart("signalAssignment");
      assignment(parm, p->assignment, p->target, ast::ObjectType::SIGNAL, callback);
      debug.functionEnd("signalAssignment");
    }
  }
}
