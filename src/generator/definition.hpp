#include "systemc.hpp"

namespace generator {


  template <typename Func>
  void SystemC::defineObject(parameters& parm,
                             std::string name,
                             std::string type,
                             std::string* argument,
                             ast::List<ast::Declaration>* declarationList,
                             ast::List<ast::ConcurrentStatement>* concurrentStatements,
                             Func bodyCallback) {
    functionStart("defineObject");
    println(parm, type + "(" + name + ") {");
    parm.incIndent();
    std::string parentName = parm.database.getParentName();
    if (parentName.size() > 0) {
      println(parm, parentName + "* p = NULL;");
    }
    if (declarationList) {
      declarations(parm, *declarationList);
    }
    println(parm, "public:");
    if (concurrentStatements) {
      parameters p = descendHierarchy(parm, name);
      concurrentStatementsDefinition(p, *concurrentStatements);
    }
    bodyCallback(parm);
    createConstructor(parm, name, argument, concurrentStatements);
    parm.decIndent();
    println(parm, "};");
    functionEnd("defineObject");
  }

}
