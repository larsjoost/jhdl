#include <cassert>
#include <algorithm>

#include "systemc.hpp"

namespace generator {

  void SystemC::loadPackage(parameters& parm, std::string package,
                            std::string library, std::string identifier,
                            ast::Text* text) {
    debug.functionStart("loadPackage(library = " + library + ", name = " + package + ")");
    bool packageExists = a_database.exists(library, package);
    if (!packageExists) {
      exceptions.printNote("Loading package " + library + "." + package);
      parm.SetArea(parameters::Area::TOP);
      parsePackage(parm, package, library);
      parm.Flush();
      packageExists = a_database.exists(library, package);
    }
    if (packageExists) {
      if (!a_database.setVisible(identifier, package, library)) {
        exceptions.printError("Could not find " + identifier + " in package " + package + " of library " + library, text);
      }
    } else {
      exceptions.printError("Did not find package " + package + " in library " + library, text);
    }
    debug.functionEnd("loadPackage");
  }
  
  void SystemC::includes(parameters& parm, ast::ContextClause* contextClause, bool load) {
    if (contextClause) {
      debug.functionStart("includes");
      for (ast::UseClause& useClause : contextClause->useClauses.list) {
        assert (useClause.package);
        assert (useClause.library);
        std::string library = useClause.library->toString(true);
	std::string package = useClause.package->toString(true);
	if (!load) {
	  std::string identifier = useClause.identifier->toString(true);
	  loadPackage(parm, package, library, identifier, &useClause.package->text);
	} 
      }
      debug.functionEnd("includes");
    }    
  }

}
