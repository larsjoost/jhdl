#include "systemc.hpp"
#include "declarations.hpp"


namespace generator {

  void SystemC::topHierarchyStart(parameters& parm, std::string& library, std::string& name,
                                  ast::ObjectType type, std::string& filename) {
    if (!parm.isQuiet()) {
      std::string s;
      switch (type) {
      case ast::PACKAGE_BODY: {s = "body"; break;}
      case ast::PACKAGE : {s = "package"; break;}
      case ast::ENTITY : {s = "entity"; break;}
      case ast::ARCHITECTURE : {s = "architecture"; break; }
      default: assert(false);
      }
      libraryInfo.add(s, name, filename);
    }
    a_database.topHierarchyStart(library, name, type);
    if (verbose) {
      parm.println("// Hierarchy top start, library = " + library +
                   ", name = " + name + ", type = " + ast::toString(type) +
                   ", hierarchy = " + std::to_string(a_database.getHierarchyLevel()));
    }
  }
  
  void SystemC::topHierarchyEnd(parameters& parm, bool globalize) {
    if (verbose) {
      parm.println("// Hierarchy top end = " + std::to_string(a_database.getHierarchyLevel()));
    }
    a_database.topHierarchyEnd(globalize);
  }

  void SystemC::descendHierarchy(parameters& parm, std::string parentName) {
    a_database.descendHierarchy(parentName);
    if (verbose) {
      parm.println("// Hierarchy start = " + std::to_string(a_database.getHierarchyLevel()));
    }
  }

  void SystemC::ascendHierarchy(parameters& parm) {
    if (verbose) {
      parm.println("// Hierarchy end = " + std::to_string(a_database.getHierarchyLevel()));
    }
    a_database.ascendHierarchy();
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
    auto func = [](std::string& type, ast::ObjectType id,
                   ast::ObjectDeclaration::Direction direction) {
      return type;
    };
    return interfaceListToString(parm, l, delimiter, initialization, func);
  }


}
