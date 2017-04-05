#include "systemc.hpp"
#include "declarations.hpp"


namespace generator {

  void SystemC::println(parameters& parm, std::string text) {
    if (!quiet) {
      std::cout << std::string(parm.indent, ' ') << text << std::endl;
    }
  }

  void SystemC::printError(std::string message, ast::Text* t) {
    if (t) {
      t->printException("error", message);
    } else {
      std::cerr << "#Error: " << message << std::endl;
    }
  }
  
  void SystemC::printWarning(std::string message, ast::Text* t) {
    if (t) {
      t->printException("warning", message, std::cout, "// ");
    } else {
      std::cerr << "#Warning: " << message << std::endl;
    }
  }

  void SystemC::functionStart(std::string name) {
    if (verbose) {
      std::cout << std::endl << "[FUNCTION START] " << name << std::endl;
    }
  }

  void SystemC::functionEnd(std::string name) {
    if (verbose) {
      std::cout << std::endl << "[FUNCTION END] " << name << std::endl;
    }
  }

    void SystemC::printDeclaration(parameters& parm) {
    for (auto i : parm.declaration) {
      std::cout << "name = " << i.first << ", hierarchy level = " << i.second.hierarchyLevel << std::endl;
    }
  }
  
  SystemC::parameters SystemC::descendHierarchy(parameters& parm, std::string& parentName) {
    parameters p = parm;
    for (auto it = p.declaration.begin(); it != p.declaration.end(); it++) {
      if (it->second.hierarchyLevel >= 0) {
        it->second.hierarchyLevel++;
      }
    }
    p.parentName = parentName;
    return p;
  }

  void SystemC::printSourceLine(parameters& parm, ast::Text& t) {
    println(parm, "// line " + std::to_string(t.getLine()) + ": " + t.getCurrentLine());
  }
  
  void SystemC::printSourceLine(parameters& parm, ast::BasicIdentifier* t) {
    printSourceLine(parm, t->text);
  }

  void SystemC::printSourceLine(parameters& parm, ast::SimpleIdentifier* t) {
    printSourceLine(parm, t->text);
  }

  std::string SystemC::interfaceListToString(parameters& parm, ast::InterfaceList* l, std::string delimiter,
                                             bool initialization) {
    return interfaceListToString(parm, l, delimiter, initialization,
                                 [](std::string& type, DeclarationID id, ast::ObjectDeclaration::Direction direction) {
                                   return type;
                                 });
  }


}
