#include <cassert>
#include <algorithm>

#include "systemc.hpp"

namespace generator {

  void SystemC::loadPackage(std::string package,
                            std::string library, std::string identifier,
                            ast::Text* text) {
    debug.functionStart("loadPackage(library = " + library + ", name = " + package + ")");
    bool packageExists = a_database.exists(library, package);
    if (!packageExists) {
      exceptions.printNote("Loading package " + library + "." + package);
      parameters parm;
      parsePackage(parm, package, library);
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
	  if ("WORK" != library) {
	    parm.println(library + "::" + package + " " + library + "_" + package + ";");
	  }
	  std::string identifier = useClause.identifier->toString(true);
	  loadPackage(package, library, identifier, &useClause.package->text);
	} else {
	  std::string p = package;
	  transform(p.begin(), p.end(), p.begin(), tolower);
	  parm.println("#include \"" + p + ".hpp\"");
	}
      }
      debug.functionEnd("includes");
    }    
  }

}
