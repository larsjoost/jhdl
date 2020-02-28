#include "systemc.hpp"

namespace generator {


  template <typename BodyFunc, typename DeclFunc>
  void SystemC::defineObject(parameters& parm,
                             bool topHierarchy,
                             std::string name,
                             ast::ObjectType type,
			     std::string base_name,
			     std::string class_description,
			     std::string* argument,
                             ast::List<ast::Declaration>* declarationList,
                             ast::List<ast::ConcurrentStatement>* concurrentStatements,
                             BodyFunc bodyCallback,
			     DeclFunc declarationCallback,
                             bool wait_statements,
                             bool init_enable) {
    debug.functionStart("defineObject(name = " + name + ", base_name = " + base_name + ")");
    openHierarchy(parm, name, type, class_description, base_name);
    ParentInfo parent_info;
    parm.getParent(parent_info);
    if (!topHierarchy) {
      parm.addClassContents(ObjectName(parent_info) + "* p = NULL; // Used to access parent class.");
    }
    debug.debug("Declaration");
    if (declarationList) {
      declarations(parm, *declarationList);
    }
    declarationCallback(parm);
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

}
