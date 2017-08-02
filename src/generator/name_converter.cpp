
#include "name_converter.hpp"

namespace generator {

  std::string NameConverter::getPrefix(DatabaseResult& object, std::string first_separator, std::string last_separator) {
    std::string library = a_database->getLibrary();
    std::string sectionName = a_database->getName();
    std::string name;
    if ((library != object.object->library) || (sectionName != object.object->sectionName)) {
      name = object.object->library + first_separator + object.object->sectionName + last_separator;
    }
    return name;
  }
  
  std::string NameConverter::getName(DatabaseResult& object, bool fullName) {
    a_debug.functionStart("getName");
    std::string name = object.object->name;
    a_debug.debug("name = " + name);
    int hierarchyLevel = a_database->getHierarchyLevel();
    a_debug.debug("Object = " + object.toString());
    if (fullName) {
      if (object.object->id == ast::ObjectType::ENUM
          // && object.object->type.value == ast::ObjectValue::ENUMERATION
          ) {
        name = getPrefix(object, "::", "::") + object.object->type.typeName + "_enum::" + name;
      } else {
        if (object.local) {
          for (int i=object.object->hierarchyLevel; i < hierarchyLevel; i++) {
            name = "p->" + name;
          }
        } else {
          if (object.object->id == ast::ObjectType::TYPE) {
            name = getPrefix(object, "::", "::") + name + "<>";
          } else {
            name = getPrefix(object, "_", ".") + name;
          }
        }
      }
    }
    a_debug.functionEnd("getName");
    return name;
  }

}
