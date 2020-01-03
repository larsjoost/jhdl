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
      if (a_database.findOne(object, iteration->identifier)) {  
        typeName = a_name_converter.GetName(object);
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
    a_database.add(ast::ObjectType::VARIABLE, name, type);
    std::string t = "for (" +
      name + " = " + name + ".LEFT(); " +
      name + " <= " + name + ".RIGHT(); " +
      name + " = " + name + ".RIGHTOF()) {";
    if (constructor) {
      parm.addClassConstructorContents(t);
    } else {
      parm.addClassContents(t);
    }
    callback(parm);
    if (constructor) {
      parm.addClassConstructorContents("}");
    } else {
      parm.addClassContents("}");
    }
    debug.functionEnd("forLoop");
  }  

  template<typename Func>
  void SystemC::assignment(parameters& parm, ast::Assignment* p, ast::BasicIdentifier* target, ast::ObjectType object_type, Func callback) {
    std::string command = "if";
    std::string noConditionCommand = "";
    std::string noConditionDelimiter = "";

    std::string name = a_expression.BasicIdentifierToString(target);
    parm.addImplementationContents(getSourceLine(target));
    ast::ReturnTypes return_types;
    a_expression.BasicIdentifierReturnTypes(target, return_types);
    ast::ObjectValueContainer expectedType;
    if (a_expression.UniqueReturnType(return_types, expectedType, &target->getText())) {
      for (ast::AssignmentCondition s : p->assignment_conditions.list) {
        if (s.condition) {
          static ast::ObjectValueContainer expectedValue(ast::ObjectValue::BOOLEAN);
          parm.addImplementationContents(command + " (" + a_expression.toString(s.condition, expectedValue, callback) + ") {");
          command = "else if";
          noConditionCommand = "else {";
          noConditionDelimiter = "}";
        } else {
          parm.addImplementationContents(noConditionCommand);
        }
        parm.addImplementationContents(name + " = " + a_expression.toString(s.expression, expectedType, callback) + ";");
        if (s.condition) {
          parm.addImplementationContents("}");
        } else {
          parm.addImplementationContents(noConditionDelimiter);
        }
      }
    } else {
      exceptions.printError("Could not find definition of " + ast::toString(object_type) + " \"" + name + "\"", &target->getText());
      a_database.printAllObjects(name);
    }
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
