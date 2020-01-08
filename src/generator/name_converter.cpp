#include <algorithm>

#include "name_converter.hpp"

namespace generator {

  std::string NameConverter::getPrefix(parameters& parm, DatabaseResult& object, std::string first_separator, std::string last_separator) {
    std::string library = parm.getLibrary();
    std::string sectionName = parm.getName();
    std::string name;
    if (!object.local) {
      name = object.library;
      for (auto& i : object.hierarchy) {
	name += first_separator + i;
      }
      name += last_separator;
    }
    return name;
  }

  std::string NameConverter::globalPrefix(parameters& parm, DatabaseResult& object, bool factory_extension) {
    Debug<false> a_debug("NameConverter::globalPrefix");
    a_debug.functionStart("GlobalPrefix");
    std::string prefix;
    int hierarchyLevel = parm.getHierarchyLevel();
    a_debug.debug("Object = " + object.toString());
    if (object.object->id == ast::ObjectType::ENUM) {
      prefix = getPrefix(parm, object, "::", "::") + object.object->type.GetTypeName() + "_enum::";
    } else {
      if (object.local) {
        if (factory_extension || object.object->id != ast::ObjectType::TYPE) {
          for (int i=object.hierarchy.size(); i < hierarchyLevel; i++) {
            prefix = "p->" + prefix;
          }
        }
      } else {
        if (!factory_extension && object.object->id == ast::ObjectType::TYPE) {
          prefix = getPrefix(parm, object, "::", "::");
        } else {
          prefix = getPrefix(parm, object, "_", ".");
        }
      }
    }
    if (factory_extension && object.object->id == ast::ObjectType::TYPE) {
      prefix += "factory_";
    } 
    a_debug.functionEnd("GlobalPrefix");
    return prefix;
  }

  std::string NameConverter::getName(parameters& parm, DatabaseResult& object, bool factory_extension, std::string factory_arguments) {
    Debug<false> a_debug("NameConverter::getName");
    a_debug.functionStart("getName");
    assert(object.object != NULL);
    std::string name = object.object->name;
    a_debug.debug("name = " + name);
    a_debug.debug("Object = " + object.toString());
    name = globalPrefix(parm, object, factory_extension) + name;
    if (factory_extension && object.object->id == ast::ObjectType::TYPE) {
      name += ".create(" + factory_arguments + ")";
    } 
    a_debug.functionEnd("getName");
    return name;
  }

  std::string NameConverter::toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
  }

  std::string NameConverter::toUpper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
  }
  
  std::string NameConverter::getName(std::string& name, ast::ObjectArguments& arguments,
                                     ast::ObjectValueContainer& return_type) {
    std::string s = name;
    for (ast::ObjectArgument& o : arguments.list) {
      s += "_" + toLower(o.type_name);
    }
    s += "__" + ast::toString(return_type.GetValue());
    return s;
  }
 
 
}
