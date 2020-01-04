#include "systemc.hpp"

namespace generator {


  template <typename BodyFunc, typename DeclFunc>
  void SystemC::defineObject(parameters& parm,
                             bool topHierarchy,
                             std::string name,
                             ast::ObjectType type,
			     std::string class_description,
			     std::string* argument,
                             ast::List<ast::Declaration>* declarationList,
                             ast::List<ast::ConcurrentStatement>* concurrentStatements,
                             BodyFunc bodyCallback,
			     DeclFunc declarationCallback,
                             bool wait_statements,
                             bool init_enable) {
    debug.functionStart("defineObject");
    openHierarchy(parm, name, type, class_description, name);
    ParentInfo parent_info;
    a_database.GetParent(parent_info);
    if (!topHierarchy) {
      parm.addClassContents(ObjectName(parent_info) + "* p = NULL; // Used to access parent class.");
    }
    debug.debug("Declaration");
    declarationCallback(parm);
    if (declarationList) {
      declarations(parm, *declarationList);
    }
    createConstructor(parm, topHierarchy, type, name, argument, concurrentStatements);
    if (concurrentStatements) {
      concurrentStatementsDefinition(parm, *concurrentStatements);
    }
    bodyCallback(parm);
    if (init_enable) {
      debug.debug("Constructor");
      if (concurrentStatements) {
        concurrentStatementsInstantiation(parm, *concurrentStatements);
      }
    }
    closeHierarchy(parm);
    debug.functionEnd("defineObject");
  }

  template<typename Func>
  void SystemC::printSensitivityListWait(parameters& parm, auto sensitivity, Func func) {
    std::string s = listToString(parm, sensitivity, " << ", [&](auto s){return func(s);}); 
    if (!s.empty()) {
      parm.addClassConstructorContents("sensitive " + s + ";");
    }
  }

}
