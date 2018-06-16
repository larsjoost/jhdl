







#include "systemc.hpp"
#include "expression/expression.hpp"

namespace generator {

  template<typename Func>
  void SystemC::forLoop(parameters& parm, std::string& name, ast::IterationScheme* iteration, Func callback) {
    debug.functionStart("forLoop");
    std::string typeName;
    ast::ObjectValueContainer type;
    if (iteration->range) {
      ast::RangeType* r = iteration->range;
      typeName = name + "_type";
      type = ast::ObjectValueContainer(ast::ObjectValue::INTEGER);
      printRangeType(parm, typeName, r);
      parm.println(parameters::Area::DECLARATION, typeName + " " + name + ";");
      std::string factory_name = "factory_" + typeName + ".create()";
      parm.println(parameters::Area::CONSTRUCTOR, name + ".construct(" + factory_name + ");");
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
    parameters::Area area =
      (parm.IsArea(parameters::Area::CONSTRUCTOR) ? parameters::Area::CONSTRUCTOR : parameters::Area::IMPLEMENTATION);
    parm.println(area, "for (" +
                 name + " = " + name + ".LEFT(); " +
                 name + " <= " + name + ".RIGHT(); " +
                 name + " = " + name + ".RIGHTOF()) {");
    callback(parm);
    parm.println(area, "}");
    debug.functionEnd("forLoop");
  }  

  template<typename Func>
  void SystemC::assignment(parameters& parm, ast::Assignment* p, ast::Target* target, ast::ObjectType object_type, Func callback) {
    std::string command = "if";
    std::string noConditionCommand = "";
    std::string noConditionDelimiter = "";

    PrintSourceLine(parm, target->identifier, parameters::Area::IMPLEMENTATION);
    std::string name = target->identifier->toString(true);
    DatabaseResult object;
    if (a_database.findOne(object, name, object_type)) {  
      std::string name = a_name_converter.GetName(object);
      for (ast::AssignmentCondition s : p->assignment_conditions.list) {
        if (s.condition) {
          static ast::ObjectValueContainer expectedValue(ast::ObjectValue::BOOLEAN);
          parm.println(parameters::Area::IMPLEMENTATION, command + " (" + a_expression.toString(s.condition, expectedValue, callback) + ") {");
          command = "else if";
          noConditionCommand = "else {";
          noConditionDelimiter = "}";
        } else {
          parm.println(parameters::Area::IMPLEMENTATION, noConditionCommand);
        }
        parm.println(parameters::Area::IMPLEMENTATION, name + " = " + a_expression.toString(s.expression, object.object->type, callback) + ";");
        if (s.condition) {
          parm.println(parameters::Area::IMPLEMENTATION, "}");
        } else {
          parm.println(parameters::Area::IMPLEMENTATION, noConditionDelimiter);
        }
      }
    } else {
      exceptions.printError("Could not find definition of " + ast::toString(object_type) + " \"" + name + "\"", &target->identifier->text);
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
