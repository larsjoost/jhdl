#include <iostream>

#include "general.hpp"

namespace generator {

  void DatabaseElement::print() {
    std::cout << "      Hierarchy = " << hierarchyLevel << std::endl;
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
    std::string attr = (attribute ? " [ATTRIBUTE]" : "");
    return ast::toString(id) + " " + library + "." + sectionName + "." +
      name + args + ": " + type.toString() + attr;
  }

  std::string DatabaseResult::toString() {
    return object ? object->toString() : "NIL";
  }

}
