#include "systemc.hpp"
#include "expression/expression.hpp"

namespace generator {

  template<typename Func>
  void SystemC::forLoop(parameters& parm, std::string& name, ast::RangeType* r, Func callback) {
    std::string typeName = name + "_type";
    if (parm.isArea(parameters::DECLARATION)) {
      database.add(ast::VARIABLE, name, ast::INTEGER);
      printRangeType(parm, typeName, r);
      parm.println(typeName + "<> " + name + ";");
    }
    if (parm.isArea(parameters::IMPLEMENTATION)) {
      parm.println("for (" +
		   name + " = " + name + ".LEFT(); " +
		   name + " <= " + name + ".RIGHT(); " +
		   name + " = " + typeName + "<>::RIGHTOF(" + name + ")) {");
      parm.incIndent();
    }
    callback(parm);
    if (parm.isArea(parameters::IMPLEMENTATION)) {
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
      if (database.findOne(object, name, ast::SIGNAL)) {  
        std::string command = "if";
        std::string noConditionCommand = "";
        std::string noConditionDelimiter = "";
        name = object.getName(true, database.getHierarchyLevel());
        ExpressionParser expr(&database);
        for (ast::SignalAssignmentCondition s : p->signalAssignmentConditions.list) {
          if (s.condition) {
            static ast::ObjectValueContainer expectedValue(ast::BOOLEAN);
            parm.println(command + " (" + expr.toString(s.condition, expectedValue, callback) + ") {");
            command = "else if";
            noConditionCommand = "else {";
            noConditionDelimiter = "}";
          } else {
            parm.println(noConditionCommand);
          }
          parm.incIndent();
          parm.println(name + " = " + expr.toString(s.expression, object.object->type, callback) + ";");
          parm.decIndent();
          if (s.condition) {
            parm.println("}");
          } else {
            parm.println(noConditionDelimiter);
          }
        }
      } else {
        exceptions.printError("Could not find definition of signal \"" + name + "\"", &p->identifier->text);
	database.printAllObjects(name);
      }
      debug.functionEnd("signalAssignment");
    }
  }
}
