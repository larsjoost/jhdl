#include <string>
#include "object_type.hpp"

namespace ast {

  std::string toString(ObjectType o) {
    static std::string a[NUMBER_OF_OBJECT_TYPES] = {
      "SIGNAL", "VARIABLE", "CONSTANT", "FUNCTION", "PROCEDURE", "PORT", "TYPE",
      "ENUM", "ENTITY", "CONFIGURATION", "SUBTYPE", "FILE", "GROUP", "ARCHITECTURE",
      "PACKAGE", "COMPONENT"};
    return a[o];
  }
  
}
