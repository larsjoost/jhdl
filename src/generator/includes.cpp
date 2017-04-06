#include <cassert>
#include <algorithm>

#include "systemc.hpp"

namespace generator {

  void SystemC::makeVisible(std::string& identifier,
                            std::string& package,
                            ast::Text* text) {
    if ("ALL" == identifier) {
      if (!database.setAllVisible(package)) {
        printError("Could not find package " + package, text);
      }
    } else {
      if (!database.setVisible(package, identifier)) {
        printError("Could not find package " + package, text);
      }
    }
  };

  void SystemC::loadPackage(parameters& parm, std::string package,
                            std::string library, std::string identifier,
                            ast::Text* text) {
    if (!database.exists(package)) {
      parsePackage(parm, package, library);
    }
    if (database.exists(package)) {
      makeVisible(identifier, package, text);
    } else {
      printError("Did not find package " + package, text);
    }
  }
  
  void SystemC::includes(parameters& parm, ast::ContextClause* contextClause) {
    if (contextClause) {
      functionStart("includes");
      for (ast::UseClause& useClause : contextClause->useClauses.list) {
        assert (useClause.package);
        assert (useClause.library);
        std::string library = useClause.library->toString(true);
        if ("WORK" != library) {
          println(parm, "using namespace " + library + ";");
        }
        std::string package = useClause.package->toString(true);
        std::string identifier = useClause.identifier->toString(true);
        loadPackage(parm, package, library, identifier, &useClause.package->text);
      }
      functionEnd("includes");
    }    
  }

}
