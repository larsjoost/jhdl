#include <cassert>
#include <algorithm>

#include "systemc.hpp"

namespace generator {

  void SystemC::loadPackage(parameters& parm, std::string package,
                            std::string library, std::string identifier,
                            ast::Text* text) {
    m_debug.functionStart("loadPackage(library = " + library + ", name = " + package + ")", false, __FILE__, __LINE__);
    std::shared_ptr<LocalDatabase> object;
    bool packageExists = parm.findObject(object, library, package, ast::ObjectType::PACKAGE);
    if (!packageExists) {
      exceptions.printNote("Loading package " + library + "." + package);
      parm.m_file_info.write_package_dependency(package, library);
      parsePackage(parm, package, library);
      packageExists = parm.findObject(object, library, package, ast::ObjectType::PACKAGE);
    }
    if (packageExists) {
      if (!parm.setVisible(identifier, package, library)) {
        exceptions.printError("Could not find " + identifier + " in package " + package + " of library " + library, text);
      }
    } else {
      exceptions.printError("Did not find package " + package + " in library " + library, text);
    }
    m_debug.functionEnd("loadPackage");
  }
  
  void SystemC::includes(parameters& parm, ast::ContextClause* contextClause, bool load) {
    if (contextClause) {
      m_debug.functionStart("includes", false, __FILE__, __LINE__);
      for (ast::UseClause& useClause : contextClause->useClauses.list) {
        assert (useClause.package);
        assert (useClause.library);
        std::string library = useClause.library->toString(true);
	std::string package = useClause.package->toString(true);
	if (!load) {
	  std::string identifier = (useClause.all ? "ALL" : useClause.identifier->toString(true));
	  loadPackage(parm, package, library, identifier, &useClause.package->text);
	} 
      }
      m_debug.functionEnd("includes");
    }    
  }

}
