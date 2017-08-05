#include "systemc.hpp"

namespace generator {


  template <typename BodyFunc, typename DeclFunc>
  void SystemC::defineObject(parameters& parm,
                             bool topHierarchy,
                             std::string name,
                             std::string type,
			     std::string* argument,
                             ast::List<ast::Declaration>* declarationList,
                             ast::List<ast::ConcurrentStatement>* concurrentStatements,
                             BodyFunc bodyCallback,
			     DeclFunc declarationCallback,
                             bool wait_statements) {
    debug.functionStart("defineObject");
    if (!topHierarchy) {descendHierarchy(parm, name);}
    parm.println(type + "(" + name + ") {");
    parm.incIndent();
    if (wait_statements) {
      parm.println("// Wait is support class of wait statements"); 
      parm.println("Wait w;");
    }
    std::string parentName = a_database.getParentName();
    if (!topHierarchy) {
      parm.println("// p is used to access parent class");
      parm.println(parentName + "* p = NULL;");
    }
    parm.decIndent();
    parm.println("public:");
    parm.incIndent();
    if (declarationList) {
      declarations(parm, *declarationList);
    }
    declarationCallback(parm);
    if (concurrentStatements) {
      concurrentStatementsDefinition(parm, *concurrentStatements);
    }
    bodyCallback(parm);
    createConstructor(parm, topHierarchy, type, name, argument, concurrentStatements);
    parm.decIndent();
    parm.println("};");
    if (!topHierarchy) {ascendHierarchy(parm);}
    debug.functionEnd("defineObject");
  }

  template<typename Func>
  void SystemC::printSensitivityListWait(parameters& parm, auto sensitivity, Func func) {
    std::string s = listToString(parm, sensitivity, " || ", [&](auto s){return func(s) + ".EVENT()";}); 
    if (s.size() > 0) {
      parm.println("// Sensitivity list");
      parm.println("if (!(" + s + ")) {return;}");
    }
  }

}
