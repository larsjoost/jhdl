#include "systemc.hpp"

namespace generator {


  template <typename BodyFunc, typename DeclFunc>
  void SystemC::DefineObject(parameters& parm,
                             bool topHierarchy,
                             std::string name,
                             ast::ObjectType type,
                             std::string derived_classes,
			     std::string* argument,
                             ast::List<ast::Declaration>* declarationList,
                             ast::List<ast::ConcurrentStatement>* concurrentStatements,
                             BodyFunc bodyCallback,
			     DeclFunc declarationCallback,
                             bool wait_statements) {
    debug.functionStart("DefineObject");
    if (!topHierarchy) {descendHierarchy(parm, name, parameters::Area::DECLARATION, type);}
    parm.println("struct " + ast::toString(type, true) + "_" + name + (derived_classes.empty() ? "" : " : public " + derived_classes) + " {");
    parm.incIndent();
    if (wait_statements) {
      parm.println("Wait w; // Support class of wait statements");
    }
    ParentInfo parent_info;
    a_database.GetParent(parent_info);
    if (!topHierarchy) {
      parm.println(ast::toString(parent_info.type, true) + "_" + parent_info.name + "* p = NULL; // Used to access parent class.");
    }
    parm.SetArea(parameters::Area::DECLARATION);
    debug.debug("Declaration");
    if (declarationList) {
      declarations(parm, *declarationList);
    }
    declarationCallback(parm);
    parm.SetArea(parameters::Area::CONSTRUCTOR);
    createConstructor(parm, topHierarchy, type, name, argument, concurrentStatements);
    parm.Flush(parameters::Area::DECLARATION);
    parm.SetArea(parameters::Area::IMPLEMENTATION);
    if (concurrentStatements) {
      concurrentStatementsDefinition(parm, *concurrentStatements);
    }
    bodyCallback(parm);
    parm.SetArea(parameters::Area::CONSTRUCTOR);
    debug.debug("Constructor");
    parm.println("void init() {");
    parm.Flush(parameters::Area::CONSTRUCTOR);
    if (concurrentStatements) {
      parm.incIndent();
      concurrentStatementsInstantiation(parm, *concurrentStatements);
      parm.decIndent();
    }
    parm.println("}");
    parm.decIndent();
    parm.Flush(parameters::Area::DECLARATION);
    parm.println("};");
    if (!topHierarchy) {ascendHierarchy(parm);}
    debug.functionEnd("DefineObject");
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
