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
      parm.addTop("// Hierarchy top start, library = " + library +
		  ", name = " + name + ", type = " + ast::toString(type) +
		  ", hierarchy = " + std::to_string(a_database.getHierarchyLevel()));
    }
  }
  
  void SystemC::topHierarchyEnd(parameters& parm, bool globalize) {
    if (verbose) {
      parm.addBottom("// Hierarchy top end = " + std::to_string(a_database.getHierarchyLevel()));
    }
    a_database.topHierarchyEnd(globalize);
  }

  void SystemC::openHierarchy(parameters& parm, std::string parent_name, ast::ObjectType parent_type, std::string class_description, std::string name) {
    debug.functionStart("openHierarchy");
    parm.newClass(class_description, name);
    a_database.openHierarchy(parent_name, parent_type);
    if (verbose) {
      parm.addTop("// Hierarchy start = " + std::to_string(a_database.getHierarchyLevel()));
    }
    debug.functionEnd("openHierarchy");
  }

  void SystemC::closeHierarchy(parameters& parm) {
    debug.functionStart("closeHierarchy");
    if (verbose) {
      parm.addBottom("// Hierarchy end = " + std::to_string(a_database.getHierarchyLevel()));
    }
    parm.endClass();
    a_database.closeHierarchy();
    debug.functionEnd("closeHierarchy");
  }

  std::string SystemC::getSourceLine(ast::Text* t) {
    assert(t);
    return "// line " + std::to_string(t->getLine()) + ": " + t->getCurrentLine();
  }

  std::string SystemC::getSourceLine(ast::Text& t) {
    return getSourceLine(&t);
  }
  
  std::string SystemC::getSourceLine(ast::BasicIdentifier* t) {
    return getSourceLine(t->getText());
  }

  std::string SystemC::getSourceLine(ast::SimpleIdentifier* t) {
    return getSourceLine(t->text);
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
