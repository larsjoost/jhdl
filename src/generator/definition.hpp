#include "systemc.hpp"

namespace generator {


  template <typename BodyFunc, typename DeclFunc>
  void SystemC::defineObject(parameters& parm,
                             std::string name,
                             std::string type,
			     std::string* argument,
                             ast::List<ast::Declaration>* declarationList,
                             ast::List<ast::ConcurrentStatement>* concurrentStatements,
                             BodyFunc bodyCallback,
			     DeclFunc declarationCallback) {
    functionStart("defineObject");
    descendHierarchy(parm, name);
    parm.println(type + "(" + name + ") {");
    parm.incIndent();
    parm.println("// Wait is support class of wait statements"); 
    parm.println("Wait w;");
    std::string parentName = database.getParentName();
    if (parentName.size() > 0) {
      parm.println("// p is used to access parent class");
      parm.println(parentName + "* p = NULL;");
    }
    if (declarationList) {
      declarations(parm, *declarationList);
    }
    declarationCallback(parm);
    parm.decIndent();
    parm.println("public:");
    parm.incIndent();
    if (concurrentStatements) {
      concurrentStatementsDefinition(parm, *concurrentStatements);
    }
    bodyCallback(parm);
    createConstructor(parm, name, argument, concurrentStatements);
    parm.decIndent();
    parm.println("};");
    ascendHierarchy(parm);
    functionEnd("defineObject");
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
