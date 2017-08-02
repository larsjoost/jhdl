#include "systemc.hpp"
#include "expression/expression.hpp"

namespace generator {

  template<typename Func>
  void SystemC::forLoop(parameters& parm, std::string& name, ast::RangeType* r, Func callback) {
    std::string typeName = name + "_type";
    if (parm.isArea(parameters::Area::DECLARATION)) {
      a_database.add(ast::ObjectType::VARIABLE, name, ast::ObjectValue::INTEGER);
      printRangeType(parm, typeName, r);
      parm.println(typeName + "<> " + name + ";");
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
