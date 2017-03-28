#include <cassert>

#include "systemc.hpp"

namespace generator {

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
        auto i = packageInfo.find(package);
        if (i != packageInfo.end()) {
          std::string identifier = useClause.identifier->toString(true);
          if ("ALL" == identifier) {
            for (auto j : i->second) {
              visiblePackageInfo[j.first] = j.second; 
            }
          } else {
            auto j = i->second.find(identifier);
            if (j != i->second.end()) {
              visiblePackageInfo[j->first] = j->second;
            } else {
              printError(useClause.identifier->text, "Did not find " + identifier + " is package " + package);
            }
          }
        } else {
          printError(useClause.package->text, "Did not find package " + package);
        }
      }
      functionEnd("includes");
    }    
  }

}
