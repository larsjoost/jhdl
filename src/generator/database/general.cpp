#include <iostream>

#include "general.hpp"

namespace generator {

  void DatabaseElement::print() {
    std::cout << toString() << std::endl;
  }

  std::string DatabaseElement::toString() {
    std::string args = arguments.toString();
    args = args.empty() ? "" : "(" + args + ")";
    std::string attr = (attribute ? " [ATTRIBUTE]" : "");
    std::string v = (visible ? "visible" : "hidden");
    return ast::toString(id) + " " + name + args + ": " + type.toString() + attr + " (" + v + ")";
  }

  std::string DatabaseResult::toString() {
    std::string s = object ? (hierarchyToString("", ".") + " " + object->toString()) : "NIL";
    return s;
  }

  std::string DatabaseResult::hierarchyToString(std::string first_delimiter, std::string delimiter) {
    std::string result;
    if (hierarchy.use_count() > 0) {
      std::string d = first_delimiter;
      for (auto& i : *hierarchy.get()) {
	result += d + i;
	d = delimiter;
      }
    }
    return result;
  };

  int DatabaseResult::hierarchySize() {
    return hierarchy.get()->size();
  }

}
