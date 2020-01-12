#include <iostream>

#include "general.hpp"

namespace generator {

  void DatabaseElement::print() {
    std::cout << "      name      = " << name << std::endl;
    std::string a = arguments.toString();
    if (!a.empty()) {
      std::cout << "      arguments = " << a << std::endl;
    }
    std::cout << "      value     = " << type.toString() << std::endl;
    if (attribute) {
      std::cout << "      attribute = true" << std::endl;
    }
    std::cout << "      type      = " << ast::toString(id) << std::endl;
  }

  std::string DatabaseElement::toString() {
    std::string args = arguments.toString();
    args = args.empty() ? "" : "(" + args + ")";
    std::string attr = (attribute ? " [ATTRIBUTE]" : "");
    return ast::toString(id) + " " + name + args + ": " + type.toString() + attr;
  }

  std::string DatabaseResult::toString() {
    return object ? object->toString() : "NIL";
  }

  std::string DatabaseResult::hierarchyToString(std::string first_delimiter, std::string delimiter) {
    std::string result;
    std::string d = first_delimiter;
    for (auto& i : *hierarchy.get()) {
	result += d + i;
	d = delimiter;
    }
    return result;
  };

  int DatabaseResult::hierarchySize() {
    return hierarchy.get()->size();
  }
  
}
