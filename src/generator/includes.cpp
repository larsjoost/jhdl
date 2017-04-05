#include <cassert>
#include <algorithm>

#include "systemc.hpp"

namespace generator {

  void SystemC::makeVisible(std::unordered_map<std::string, PackageInfo>& info,
                            std::string& identifier,
                            std::string& package,
                            ast::Text* text) {
    if ("ALL" == identifier) {
      for (auto i : info) {
        visiblePackageInfo[i.first] = i.second; 
      }
    } else {
      auto i = info.find(identifier);
      if (i != info.end()) {
        visiblePackageInfo[i->first] = i->second;
      } else {
        printError("Did not find \"" + identifier + "\" in package \"" + package + "\"", text);
      }
    }
  };

  void SystemC::loadPackage(parameters& parm, std::string package,
                            std::string library, std::string identifier,
                            ast::Text* text) {
    auto info = packageInfo.find(package);
    if (info == packageInfo.end()) {
      parsePackage(parm, package, library);
      info = packageInfo.find(package);
    }
    if (info != packageInfo.end()) {
      makeVisible(info->second, identifier, package, text);
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
