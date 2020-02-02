#include <algorithm>

#include "name_converter.hpp"

namespace generator {

  std::string NameConverter::getPrefix(parameters& parm, DatabaseResult& object,
				       std::string first_separator,
				       std::string last_separator) {
    Debug<true> a_debug("NameConverter::getPrefix");
    a_debug.functionStart("getPrefix");
    std::string name;
    name = object.hierarchyToString("", first_separator) + last_separator;
    return toUpper(name);
    /*
    std::list<std::string> current_hierarchy;
    parm.getHierarchy(current_hierarchy);
    std::list<std::string> object_hierarchy;
    for (auto& i : *object.hierarchy) {
      std::string u = toUpper(i);
      if (!name.empty()) {
	name += first_separator + u; 
      } else if (u != toUpper(current_hierarchy.front())) {
	name = u;
      } else {
	current_hierarchy.pop_front();
      }
    }
    name += last_separator;
    a_debug.functionEnd("getPrefix: " + name);
    return name;
    */
  }

  std::string NameConverter::globalPrefix(parameters& parm, DatabaseResult& object, bool factory_extension) {
    Debug<true> a_debug("NameConverter::globalPrefix");
    a_debug.functionStart("globalPrefix(object = " + object.toString() + ", factory_extension = " + std::to_string(factory_extension) + ")");
    std::string prefix;
    bool local = parm.isLocal(object);
    if (object.object->id == ast::ObjectType::ENUM) {
      if (!local) {
	prefix = getPrefix(parm, object, "::", "::");
      }
      prefix += object.object->type.GetTypeName() + "_enum::";
    } else {
      if (local) {
        if (factory_extension || object.object->id != ast::ObjectType::TYPE) {
	  int hierarchyLevel = parm.getHierarchyLevel();
	  a_debug.debug("hierarchyLevel = " + std::to_string(hierarchyLevel) +
			", current hierarchy = " + parm.hierarchyToString() +
			", object hierarchy size = " + std::to_string(object.hierarchySize()));
          for (int i=object.hierarchySize(); i < hierarchyLevel; i++) {
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
    a_debug.functionEnd("globalPrefix: " + prefix);
    return prefix;
  }

  std::string NameConverter::getName(parameters& parm, DatabaseResult& object, bool factory_extension, std::string factory_arguments) {
    Debug<true> a_debug("NameConverter::getName");
    std::string name = object.object->name;
    a_debug.functionStart("getName(name = " + name + ")");
    assert(object.object != NULL);
    a_debug.debug("Object = " + object.toString());
    name = globalPrefix(parm, object, factory_extension) + name;
    if (factory_extension && object.object->id == ast::ObjectType::TYPE) {
      name += ".create(" + factory_arguments + ")";
    } 
    a_debug.functionEnd("getName: " + name);
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
 
  std::string NameConverter::replaceFileExtension(std::string filename, std::string extension) {
    return baseName(filename) + extension;
  }

  std::string NameConverter::baseName(std::string filename) {
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos) return filename;
    return filename.substr(0, lastdot);
  }
  
 std::string NameConverter::getHeaderFileName(std::string& library, std::string& file_name) {
   return toLower(library) + "_" + replaceFileExtension(toLower(file_name), ".hpp");
 }
}
