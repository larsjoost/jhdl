#include "systemc.hpp"
#include "declarations.hpp"


namespace generator {

  void SystemC::topHierarchyStart(parameters& parm, std::string& library, std::string& name,
                                  ast::ObjectType type, std::string& filename) {
    if (!parm.isQuiet()) {
      std::string s;
      switch (type) {
      case ast::ObjectType::PACKAGE_BODY: {s = "body"; break;}
      case ast::ObjectType::PACKAGE : {s = "package"; break;}
      case ast::ObjectType::ENTITY : {s = "entity"; break;}
      case ast::ObjectType::ARCHITECTURE : {s = "architecture"; break; }
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

  void SystemC::descendHierarchy(parameters& parm, std::string parent_name, parameters::Area area,
                                 ast::ObjectType parent_type) {
    debug.functionStart("descendHierarchy");
    parm.DescendHierarchy(area);
    a_database.descendHierarchy(parent_name, parent_type);
    if (verbose) {
      parm.println("// Hierarchy start = " + std::to_string(a_database.getHierarchyLevel()));
    }
    debug.functionEnd("descendHierarchy");
  }

  void SystemC::ascendHierarchy(parameters& parm) {
    debug.functionStart("ascendHierarchy");
    if (verbose) {
      parm.println("// Hierarchy end = " + std::to_string(a_database.getHierarchyLevel()));
    }
    parm.AscendHierarchy();
    a_database.ascendHierarchy();
    debug.functionEnd("ascendHierarchy");
  }

  void SystemC::PrintSourceLine(parameters& parm, ast::Text* t, parameters::Area area) {
    assert(t);
    parm.println(area, "// line " + std::to_string(t->getLine()) + ": " + t->getCurrentLine());
  }

  void SystemC::PrintSourceLine(parameters& parm, ast::Text& t, parameters::Area area) {
    PrintSourceLine(parm, &t, area);
  }
  
  void SystemC::PrintSourceLine(parameters& parm, ast::BasicIdentifier* t, parameters::Area area) {
    PrintSourceLine(parm, t->text, area);
  }

  void SystemC::PrintSourceLine(parameters& parm, ast::SimpleIdentifier* t, parameters::Area area) {
    PrintSourceLine(parm, t->text, area);
  }

  void SystemC::PrintSourceLine(parameters& parm, ast::Text* t) {
    assert(t);
    parm.println("// line " + std::to_string(t->getLine()) + ": " + t->getCurrentLine());
  }

  void SystemC::PrintSourceLine(parameters& parm, ast::Text& t) {
    PrintSourceLine(parm, &t);
  }
  
  void SystemC::PrintSourceLine(parameters& parm, ast::BasicIdentifier* t) {
    PrintSourceLine(parm, t->text);
  }

  void SystemC::PrintSourceLine(parameters& parm, ast::SimpleIdentifier* t) {
    PrintSourceLine(parm, t->text);
  }

  std::string SystemC::interfaceListToString(parameters& parm, ast::InterfaceList* l, std::string delimiter,
                                             bool initialization, std::string local_prefix,
                                             bool database_enable) {
    auto func = [](std::string& type, ast::ObjectType id,
                   ast::ObjectDeclaration::Direction direction) {
      return type;
    };
    return interfaceListToString(parm, l, delimiter, initialization, func, local_prefix, database_enable);
  }


}
