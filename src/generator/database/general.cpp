#include <iostream>

#include "general.hpp"

namespace generator {
  
  void DatabaseElement::print() {
    std::cout << "      library   = " << library << std::endl;
    std::cout << "      package   = " << package << std::endl;
    std::cout << "      name      = " << name << std::endl;
    std::cout << "      arguments = " << arguments.toString() << std::endl;
    std::cout << "      visible   = " << (visible ? "true" : "false") << std::endl;
    std::cout << "      attribute = " << (attribute ? "true" : "false") << std::endl;
    std::cout << "      type      = " << ast::toString(id) << std::endl;
  }

  std::string DatabaseResult::getName(bool fullName) {
    std::string name = object->name;
    if (fullName) {
      if (local) {
        for (int i=0; i < hierarchyLevel; i++) {
          name = "p->" + name;
        }
      } else {
        name =
          object->library + "::" +
          object->package + "::" +
          name;
      }
    }
    return name;
  }

}
