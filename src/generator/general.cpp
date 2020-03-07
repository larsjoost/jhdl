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
    if (a_verbose) {
      parm.addTop("// Hierarchy top start, library = " + library +
		  ", name = " + name + ", type = " + ast::toString(type));
    }
  }
  
  void SystemC::topHierarchyEnd(parameters& parm, ast::Text* text, bool globalize) {
    if (a_verbose) {
      parm.addBottom("// Hierarchy top end ");
    }
    if (globalize) {
      parm.globalizeClass(text);
    }
  }

  void SystemC::openHierarchy(parameters& parm, std::string name, ast::ObjectType type, std::string class_description, std::string base_name) {
    m_debug.functionStart("openHierarchy");
    parm.newClass(class_description, name, type, base_name);
    if (a_verbose) {
      parm.addTop("// Hierarchy start");
    }
    m_debug.functionEnd("openHierarchy");
  }

  void SystemC::closeHierarchy(parameters& parm) {
    m_debug.functionStart("closeHierarchy");
    if (a_verbose) {
      parm.addBottom("// Hierarchy end");
    }
    parm.endClass();
    m_debug.functionEnd("closeHierarchy");
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
