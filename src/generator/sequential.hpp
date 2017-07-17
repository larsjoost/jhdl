#include "systemc.hpp"
#include "expression/expression.hpp"

namespace generator {

  template<typename Func>
  void SystemC::forLoop(parameters& parm, std::string& name, ast::RangeType* r, Func callback) {
    ExpressionParser expr(&database);
    std::string left = expr.toString(r->left, ast::INTEGER);
    std::string right = expr.toString(r->right, ast::INTEGER);
    std::string rangeName = name + "_range";
    parm.println("struct " + rangeName + " { int left = " + left + "; int right = " + right + "; };");
    parm.println("for (auto " + name + " : INTEGER<" + rangeName + ">()) {");
    parm.incIndent();
    callback(parm);
    parm.decIndent();
    parm.println("}");
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
    }
    functionEnd("signalAssignment");
  }
}
