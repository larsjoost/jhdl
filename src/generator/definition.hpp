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
                             bool wait_statements,
                             bool init_enable) {
    debug.functionStart("DefineObject");
    if (!topHierarchy) {descendHierarchy(parm, name, parameters::Area::DECLARATION, type);}
    parm.println("struct " + ObjectName(type, name) + (derived_classes.empty() ? "" : " : public " + derived_classes) + " {");
    if (wait_statements) {
      parm.println("Wait w; // Support class of wait statements");
    }
    ParentInfo parent_info;
    a_database.GetParent(parent_info);
    if (!topHierarchy) {
      parm.println(ObjectName(parent_info) + "* p = NULL; // Used to access parent class.");
    }
    parm.SetArea(parameters::Area::DECLARATION);
    debug.debug("Declaration");
    if (declarationList) {
      declarations(parm, *declarationList);
    }
    declarationCallback(parm);
    createConstructor(parm, topHierarchy, type, name, argument, concurrentStatements, init_enable);
    parm.SetArea(parameters::Area::DECLARATION);
    parm.SetArea(parameters::Area::IMPLEMENTATION);
    if (concurrentStatements) {
      concurrentStatementsDefinition(parm, *concurrentStatements);
    }
    bodyCallback(parm);
    parm.SetArea(parameters::Area::DECLARATION);
    if (init_enable) {
      debug.debug("Constructor");
      parm.println("void init() {");
      parm.SetArea(parameters::Area::CONSTRUCTOR);
      if (concurrentStatements) {
        concurrentStatementsInstantiation(parm, *concurrentStatements);
      }
      parm.println("}");
      parm.SetArea(parameters::Area::DECLARATION);
    }
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
