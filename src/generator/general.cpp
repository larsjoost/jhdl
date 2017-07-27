#include "systemc.hpp"
#include "declarations.hpp"


namespace generator {


  void SystemC::functionStart(std::string name) {
    if (verbose) {
      std::cout << std::endl << "[FUNCTION START] SystemC." << name << std::endl;
    }
  }

  void SystemC::functionEnd(std::string name) {
    if (verbose) {
      std::cout << std::endl << "[FUNCTION END] SystemC." << name << std::endl;
    }
  }

  void SystemC::descendHierarchy(parameters& parm, std::string parentName) {
    database.descendHierarchy(parentName);
    if (verbose) {
      parm.println("// Hierarchy start = " + std::to_string(database.getHierarchyLevel()));
    }
  }

  void SystemC::ascendHierarchy(parameters& parm) {
    if (verbose) {
      parm.println("// Hierarchy end = " + std::to_string(database.getHierarchyLevel()));
    }
    database.ascendHierarchy();
  }

  void SystemC::printSourceLine(parameters& parm, ast::Text* t) {
    assert(t);
    parm.println("// line " + std::to_string(t->getLine()) + ": " + t->getCurrentLine());
  }

  void SystemC::printSourceLine(parameters& parm, ast::Text& t) {
    printSourceLine(parm, &t);
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
                                 [](std::string& type, ast::ObjectType id, ast::ObjectDeclaration::Direction direction) {
                                   return type;
                                 });
  }


}
