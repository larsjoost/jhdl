#include "systemc.hpp"
#include "expression/expression.hpp"

namespace generator {

  template<typename Func>
  void SystemC::forLoop(parameters& parm, std::string& name, ast::RangeType* r, Func callback) {
    ExpressionParser expr(&database);
    std::string left = expr.toString(r->left, ast::INTEGER);
    std::string right = expr.toString(r->right, ast::INTEGER);
    std::string rangeName = name + "_range";
    println(parm, "struct " + rangeName + " { int left = " + left + "; int right = " + right + "; };");
    println(parm, "for (auto " + name + " : INTEGER<" + rangeName + ">()) {");
    parm.incIndent();
    callback(parm);
    parm.decIndent();
    println(parm, "}");
  }
  

  template <typename Func>
  void SystemC::signalAssignment(parameters& parm, ast::SignalAssignment* p, Func callback) {
    functionStart("signalAssignment");
    if (p) {
      printSourceLine(parm, p->identifier);
      std::string name = p->identifier->toString(true);
      DatabaseResult object;
      auto valid = [&](DatabaseElement* e) {
        return e->id == ast::SIGNAL;
      };
      if (database.findOne(object, name, valid)) {  
        std::string command = "if";
        std::string noConditionCommand = "";
        std::string noConditionDelimiter = "";
        name = object.getName(true);
        ExpressionParser expr(&database);
        for (ast::SignalAssignmentCondition s : p->signalAssignmentConditions.list) {
          if (s.condition) {
            ast::ObjectValueContainer expectedValue("BOOLEAN");
            println(parm, command + " (" + expr.toString(s.condition, expectedValue, callback) + ") {");
            command = "else if";
            noConditionCommand = "else {";
            noConditionDelimiter = "}";
          } else {
            println(parm, noConditionCommand);
          }
          parm.incIndent();
          println(parm, name + " = " + expr.toString(s.expression, object.object->type, callback) + ";");
          parm.decIndent();
          if (s.condition) {
            println(parm, "}");
          } else {
            println(parm, noConditionDelimiter);
          }
        }
      } else {
        exceptions.printError("Could not find definition of signal \"" + name + "\"", &p->identifier->text);
	database.printAllObjects(name);
      }
    }
    functionEnd("signalAssignment");
  }
}
