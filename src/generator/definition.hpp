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
    parm.SetArea(parameters::Area::DECLARATION, true);
    if (declarationList) {
      declarations(parm, *declarationList);
    }
    declarationCallback(parm);
    parm.SetArea(parameters::Area::CONSTRUCTOR);
    createConstructor(parm, topHierarchy, type, name, argument, concurrentStatements);
    parm.SetArea(parameters::Area::IMPLEMENTATION);
    if (concurrentStatements) {
      concurrentStatementsDefinition(parm, *concurrentStatements);
    }
    bodyCallback(parm);
    parm.SetArea(parameters::Area::CONSTRUCTOR);
    parm.println("void init() {");
    if (concurrentStatements) {
      parm.incIndent();
      concurrentStatementsInstantiation(parm, *concurrentStatements);
      parm.decIndent();
    }
    parm.Flush(parameters::Area::CONSTRUCTOR);
    parm.println("}");
    parm.decIndent();
    parm.println("};");
    if (!topHierarchy) {ascendHierarchy(parm);}
    assert(parm.PrintlnBuffersEmpty());
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
