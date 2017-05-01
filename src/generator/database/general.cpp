#include <iostream>

#include "general.hpp"

namespace generator {
  
  void DatabaseElement::print() {
    std::cout << "      library   = " << library << std::endl;
    std::cout << "      " << ast::toString(sectionType) << "   = " << sectionName << std::endl;
    std::cout << "      name      = " << name << std::endl;
    std::cout << "      arguments = " << arguments.toString() << std::endl;
    std::cout << "      value     = " << type.toString() << std::endl;
    std::cout << "      visible   = " << (visible ? "true" : "false") << std::endl;
    std::cout << "      attribute = " << (attribute ? "true" : "false") << std::endl;
    std::cout << "      type      = " << ast::toString(id) << std::endl;
  }

  std::string DatabaseElement::toString() {
    std::string args = arguments.toString();
    args = args.empty() ? "" : "(" + args + ")";
    return ast::toString(id) + " " + library + "." + sectionName + "." + name + args + " : " + type.toString();
  }

  std::string DatabaseResult::toString() {
    return object ? object->toString() : "NIL";
  }
  
  std::string DatabaseResult::getName(bool fullName, std::string library, std::string sectionName) {
    std::string name = object->name;
    if (fullName) {
      if (local) {
        for (int i=0; i < hierarchyLevel; i++) {
          name = "p->" + name;
        }
      } else {
        if ((library != object->library) || (sectionName != object->sectionName)) {
          name = object->library + "::" + object->sectionName + "::" + name;
        }
      }
    }
    return name;
  }

}
