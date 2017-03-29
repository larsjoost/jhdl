#include "systemc.hpp"

namespace generator {

  template<typename Func>
  void SystemC::forLoop(parameters& parm, std::string& name, ast::RangeType* r, Func callback) {
    std::string left = expressionToString(parm, r->left);
    std::string right = expressionToString(parm, r->right);
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
      std::string name = basicIdentifierToString(parm, p->identifier);
      std::string command = "if";
      std::string noConditionCommand = "";
      std::string noConditionDelimiter = "";
      for (ast::SignalAssignmentCondition s : p->signalAssignmentConditions.list) {
        if (s.condition) {
          println(parm, command + " (" + expressionToString(parm, s.condition, callback) + ") {");
          command = "else if";
          noConditionCommand = "else {";
          noConditionDelimiter = "}";
        } else {
          println(parm, noConditionCommand);
        }
        parm.incIndent();
        println(parm, name + " = " + expressionToString(parm, s.expression, callback) + ";");
        parm.decIndent();
        if (s.condition) {
          println(parm, "}");
        } else {
          println(parm, noConditionDelimiter);
        }
      }
    }
    functionEnd("signalAssignment");
  }
}
