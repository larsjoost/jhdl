#include <iostream>

#include "general.hpp"

namespace generator {

  void DatabaseElement::print() {
    std::cout << toString() << std::endl;
  }

  std::string DatabaseElement::toString() {
    std::string args = interface.toString();
    args = args.empty() ? "" : "(" + args + ")";
    std::string attr = (attribute ? " [ATTRIBUTE]" : "");
    std::string v = (visible ? "visible" : "hidden");
    return ast::toString(id) + " " + name + args + ": " + type.toString() + attr + " (" + v + ")";
  }

  int DatabaseElement::getSignalAssignmentIndex(std::string& name, ast::Text* text) {
    int current_index;
    auto signal_assignment = signal_assignments.find(name);
    if (signal_assignment != signal_assignments.end()) {
      current_index = signal_assignment->second.index++;
      signal_assignment->second.text = text;
    } else {
      SignalAssignmentContainer x;
      x.text = text;
      x.index = signal_assignments.size();
      current_index = x.index;
      signal_assignments[name] = x;
    }
    return current_index;
  }
  
  std::string DatabaseResult::toString() {
    std::string l = local ? "local" : "global"; 
    std::string s = object ? (hierarchyToString("", ".") + " " + object->toString() + " (" + l + ") [" + (valid ? "valid" : "invalid") + "] [hierarchy = " + std::to_string(hierarchySize()) + "]") : "NIL";
    return s;
  }

  void DatabaseResult::print() {
    std::cout << toString() << std::endl;
    if (object && object->text) {
      std::cout << object->text->getFilename() << "(" << object->text->getLine() << ")" << std::endl;
      std::cout << object->text->getCurrentLine() << std::endl;
      std::cout << object->text->getCurrentLinePositionMarker() << std::endl;
    }
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
    int size = -1;
    if (hierarchy.use_count() > 0) {
      size = hierarchy.get()->size();
    }
    return size;
  }

}
