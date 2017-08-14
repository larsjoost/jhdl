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
      if (parm.isArea(parameters::Area::DECLARATION)) {
        type = ast::ObjectValueContainer(ast::ObjectValue::INTEGER);
        printRangeType(parm, typeName, r);
        parm.println(typeName + "<> " + name + ";");
      }
    } else if (iteration->identifier) {
      DatabaseResult object;
      if (a_database.findOne(object, iteration->identifier)) {  
        typeName = a_name_converter.getName(object, true);
        if (iteration->range_attribute) {
          if (object.object->type.GetValue() == ast::ObjectValue::ARRAY) { 
            type = *object.object->type.GetSubtype(); 
          } else {
            exceptions.printError(name + " must be array type to use range attribute", &iteration->identifier->text);
          }
        } else {
          type = object.object->type;
        }
      } 
    }
    if (parm.isArea(parameters::Area::DECLARATION)) {
      a_database.add(ast::ObjectType::VARIABLE, name, type);
    }
    if (parm.isArea(parameters::Area::IMPLEMENTATION)) {
      parm.println("for (" +
                   name + " = " + name + ".LEFT(); " +
                   name + " <= " + name + ".RIGHT(); " +
                   name + " = " + typeName + "<>::RIGHTOF(" + name + ")) {");
      parm.incIndent();
    }
    callback(parm);
    if (parm.isArea(parameters::Area::IMPLEMENTATION)) {
      parm.decIndent();
      parm.println("}");
    }
    debug.functionEnd("forLoop");
  }  

  template <typename Func>
  void SystemC::signalAssignment(parameters& parm, ast::SignalAssignment* p, Func callback) {
    if (p) {
      debug.functionStart("signalAssignment");
      printSourceLine(parm, p->identifier);
      std::string name = p->identifier->toString(true);
      DatabaseResult object;
      if (a_database.findOne(object, name, ast::ObjectType::SIGNAL)) {  
        std::string command = "if";
        std::string noConditionCommand = "";
        std::string noConditionDelimiter = "";
        name = a_name_converter.getName(object, true);
        for (ast::SignalAssignmentCondition s : p->signalAssignmentConditions.list) {
          if (s.condition) {
            static ast::ObjectValueContainer expectedValue(ast::ObjectValue::BOOLEAN);
            parm.println(command + " (" + a_expression.toString(s.condition, expectedValue, callback) + ") {");
            command = "else if";
            noConditionCommand = "else {";
            noConditionDelimiter = "}";
          } else {
            parm.println(noConditionCommand);
          }
          parm.incIndent();
          parm.println(name + " = " + a_expression.toString(s.expression, object.object->type, callback) + ";");
          parm.decIndent();
          if (s.condition) {
            parm.println("}");
          } else {
            parm.println(noConditionDelimiter);
          }
        }
      } else {
        exceptions.printError("Could not find definition of signal \"" + name + "\"", &p->identifier->text);
	a_database.printAllObjects(name);
      }
      debug.functionEnd("signalAssignment");
    }
  }
}
