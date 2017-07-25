#include "systemc.hpp"

namespace generator {


  template <typename BodyFunc, typename DeclFunc>
  void SystemC::defineObject(parameters& parm,
                             std::string name,
                             std::string type,
			     std::string* constructor,
			     std::string* argument,
                             ast::List<ast::Declaration>* declarationList,
                             ast::List<ast::ConcurrentStatement>* concurrentStatements,
                             BodyFunc bodyCallback,
			     DeclFunc declarationCallback) {
    functionStart("defineObject");
    parm.println(type + "(" + name + ") {");
    parm.incIndent();
    parm.println("Wait w;");
    std::string parentName = database.getParentName();
    if (parentName.size() > 0) {
      parm.println(parentName + "* p = NULL;");
    }
    if (declarationList) {
      declarations(parm, *declarationList);
    }
    declarationCallback(parm);
    parm.println("public:");
    if (constructor) {
      parm.println(*constructor);
    }
    if (concurrentStatements) {
      descendHierarchy(parm, name);
      concurrentStatementsDefinition(parm, *concurrentStatements);
      ascendHierarchy(parm);
    }
    bodyCallback(parm);
    createConstructor(parm, name, argument, concurrentStatements);
    parm.decIndent();
    parm.println("};");
    functionEnd("defineObject");
  }

  template<typename Func>
  void SystemC::printSensitivityListWait(parameters& parm, auto sensitivity, Func func) {
    std::string s = listToString(parm, sensitivity, " || ", [&](auto s){return func(s) + ".EVENT()";}); 
    std::string argument = "";
    if (s.size() > 0) {
      argument = "[&](){return " + s + ";}";
    }
    parm.println("if (wait(" + argument + ")) {return;}");
  }

}
