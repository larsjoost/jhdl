
#include "systemc.hpp"

namespace generator {


  template <typename F1, typename F2>
  void SystemC::defineObject(parameters& parm,
                             bool topHierarchy,
                             std::string name,
                             ast::ObjectType type,
			     std::string base_name,
			     std::string* class_description,
			     std::string* argument,
                             ast::List<ast::Declaration>* declarationList,
                             ast::List<ast::ConcurrentStatement>* concurrentStatements,
                             F1 initial_callback,
			     F2 body_callback,
			     bool wait_statements,
                             bool init_enable) {
    m_debug.functionStart("defineObject(name = " + name + ", base_name = " + base_name + ")", false,  __FILE__, __LINE__);
    std::string c = (class_description ? *class_description : "struct " + NameConverter::objectName(type, name));
    openHierarchy(parm, name, type, c, base_name);
    parm.process_contains_wait = false;
    ParentInfo parent_info;
    
    parm.getParent(parent_info);
    if (!topHierarchy) {
      parm.addClassContents(ObjectName(parent_info) + "* p = NULL; // Used to access parent class.", __FILE__, __LINE__);
    }
    initial_callback(parm);
    m_debug.debug("Declaration");
    if (declarationList) {
      declarations(parm, *declarationList);
    }
    body_callback(parm);
    createConstructor(parm, topHierarchy, type, name, argument, concurrentStatements);
    if (concurrentStatements) {
      concurrentStatementsDefinition(parm, *concurrentStatements);
    }
    if (init_enable) {
      m_debug.debug("Constructor");
      if (concurrentStatements) {
        concurrentStatementsInstantiation(parm, *concurrentStatements);
      }
    }
    closeHierarchy(parm);
    m_debug.functionEnd("defineObject");
  }

}
