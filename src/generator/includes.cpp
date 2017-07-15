#include <cassert>
#include <algorithm>

#include "systemc.hpp"

namespace generator {

  void SystemC::loadPackage(parameters& parm, std::string package,
                            std::string library, std::string identifier,
                            ast::Text* text) {
    functionStart("loadPackage(library = " + library + ", name = " + package + ")");
    std::string lib = (library == "WORK") ? database.getLibrary() : library;
    if (!database.exists(lib, package)) {
      exceptions.printNote("Loading package " + library + "." + package);
      parsePackage(parm, package, lib);
    }
    if (database.exists(lib, package)) {
      if (!database.setVisible(identifier, package, lib)) {
        exceptions.printError("Could not find " + identifier + " in package " + package + " of library " + library, text);
      }
    } else {
      exceptions.printError("Did not find package " + package + " in library " + library, text);
    }
    functionEnd("loadPackage");
  }
  
  void SystemC::includes(parameters& parm, ast::ContextClause* contextClause, bool load) {
    if (contextClause) {
      functionStart("includes");
      for (ast::UseClause& useClause : contextClause->useClauses.list) {
        assert (useClause.package);
        assert (useClause.library);
        std::string library = useClause.library->toString(true);
	if (!load) {
	  if ("WORK" != library) {
	    println(parm, "using namespace " + library + ";");
	  }
	} else {
	  std::string package = useClause.package->toString(true);
	  std::string p = package;
	  transform(p.begin(), p.end(), p.begin(), tolower);
	  println(parm, "#include \"" + p + ".hpp\"");
	  std::string identifier = useClause.identifier->toString(true);
	  loadPackage(parm, package, library, identifier, &useClause.package->text);
	}
      }
      functionEnd("includes");
    }    
  }

}
