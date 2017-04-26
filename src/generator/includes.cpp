#include <cassert>
#include <algorithm>

#include "systemc.hpp"

namespace generator {

  void SystemC::loadPackage(parameters& parm, std::string package,
                            std::string library, std::string identifier,
                            ast::Text* text) {
    functionStart("loadPackage(library = " + library + ", name = " + package + ")");
    if (!database.exists(library, package)) {
      parsePackage(parm, package, library);
    }
    if (database.exists(library, package)) {
      if (!database.setVisible(identifier, package, library)) {
        exceptions.printError("Could not find " + identifier + " in package " + package + " of library " + library, text);
      }
    } else {
      exceptions.printError("Did not find package " + package + " in library " + library, text);
    }
    functionEnd("loadPackage");
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
