#include <algorithm>

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

  std::string NameConverter::GlobalPrefix(DatabaseResult& object, bool factory_extension) {
    a_debug.functionStart("GlobalPrefix");
    std::string prefix;
    int hierarchyLevel = a_database->getHierarchyLevel();
    a_debug.debug("Object = " + object.toString());
    if (object.object->id == ast::ObjectType::ENUM) {
      prefix = getPrefix(object, "::", "::") + object.object->type.GetTypeName() + "_enum::";
    } else {
      if (object.local) {
        if (factory_extension || object.object->id != ast::ObjectType::TYPE) {
          for (int i=object.object->hierarchyLevel; i < hierarchyLevel; i++) {
            prefix = "p->" + prefix;
          }
        }
      } else {
        if (!factory_extension && object.object->id == ast::ObjectType::TYPE) {
          prefix = getPrefix(object, "::", "::");
        } else {
          prefix = getPrefix(object, "_", ".");
        }
      }
    }
    if (factory_extension && object.object->id == ast::ObjectType::TYPE) {
      prefix += "factory_";
    } 
    a_debug.functionEnd("GlobalPrefix");
    return prefix;
  }

  std::string NameConverter::GetName(DatabaseResult& object, bool factory_extension, std::string factory_arguments) {
    a_debug.functionStart("getName");
    assert(object.object != NULL);
    std::string name = object.object->name;
    a_debug.debug("name = " + name);
    int hierarchyLevel = a_database->getHierarchyLevel();
    a_debug.debug("Object = " + object.toString());
    name = GlobalPrefix(object, factory_extension) + name;
    if (factory_extension && object.object->id == ast::ObjectType::TYPE) {
      name += ".create(" + factory_arguments + ")";
    } 
    a_debug.functionEnd("getName");
    return name;
  }

  std::string NameConverter::ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
  }

  std::string NameConverter::ToUpper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
  }
  
  std::string NameConverter::getName(std::string& name, ast::ObjectArguments& arguments,
                                     ast::ObjectValueContainer& return_type) {
    std::string s = name;
    for (ast::ObjectArgument& o : arguments.list) {
      s += "_" + ToLower(o.type_name);
    }
    s += "__" + ast::toString(return_type.GetValue());
    return s;
  }
 
  std::string NameConverter::GetName(ast::SimpleIdentifier* i, ast::ObjectType o) {
    DatabaseResult result;
    if (!a_database->findOne(result, i, o)) {
      assert(false);
    }
    return GetName(result);
  }
 
}
