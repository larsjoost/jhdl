#include <algorithm>
#include <boost/algorithm/string/replace.hpp>

#include "name_converter.hpp"

namespace generator {

  std::string NameConverter::getPrefix(parameters& parm, DatabaseResult& object,
				       std::string first_separator,
				       std::string last_separator) {
    Debug<false> debug("NameConverter::getPrefix");
    debug.functionStart("getPrefix", false, __FILE__, __LINE__);
    std::string name = object.hierarchyToString("", first_separator) + last_separator;
    debug.functionEnd("getPrefix: " + name);
    return name;
  }

  std::string NameConverter::getTopLevelPrefix(parameters& parm, int hierarchy_offset) {
    return getParentPrefix(parm, hierarchy_offset - 2);
  }
  
  std::string NameConverter::getParentPrefix(parameters& parm, int hierarchy_offset) {
    Debug<false> debug("NameConverter::getParentPrefix");
    debug.functionStart("getParentPrefix(hierarchy_offset = " + std::to_string(hierarchy_offset) + ")");
    int hierarchyLevel = parm.getHierarchyLevel();
    debug.debug("hierarchyLevel = " + std::to_string(hierarchyLevel));
    std::string prefix;
    for (int i=0; i < (hierarchyLevel + hierarchy_offset); i++) {
      prefix = "p->" + prefix;
    }
    debug.functionEnd("getParentPrefix: " + prefix);
    return prefix;
  }
  
  std::string NameConverter::globalPrefix(parameters& parm, DatabaseResult& object, bool factory_extension, bool global_scope, int hierarchy_offset) {
    Debug<false> debug("NameConverter::globalPrefix");
    debug.functionStart("globalPrefix(object = " + object.toString() + ", factory_extension = " + std::to_string(factory_extension) +
			  ", hierarchy_offset = " + std::to_string(hierarchy_offset) + ")");
    std::string prefix;
    bool local = (!global_scope) && parm.isLocal(object);
    debug.debug("Local = " + std::to_string(local));
    if (object.object->id == ast::ObjectType::ENUM) {
      if (!local) {
	prefix = getPrefix(parm, object, "::", "::");
      }
      prefix += object.object->type.GetTypeName() + "_enum::";
    } else {
      if (local) {
        if (factory_extension || object.object->id != ast::ObjectType::TYPE) {
	  debug.debug("current hierarchy = " + parm.hierarchyToString() +
			", object hierarchy size = " + std::to_string(object.hierarchySize()));
	  prefix = getParentPrefix(parm, hierarchy_offset - object.hierarchySize()) + prefix;
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
    debug.functionEnd("globalPrefix: " + prefix);
    return prefix;
  }

  std::string NameConverter::getName(parameters& parm, DatabaseResult& object, bool factory_extension, std::string factory_arguments, bool global_scope) {
    Debug<false> debug("NameConverter::getName");
    std::string name = object.object->name;
    debug.functionStart("getName(name = " + name + ")");
    assert(object.object != NULL);
    debug.debug("Object = " + object.toString());
    name = globalPrefix(parm, object, factory_extension, global_scope) + name;
    if (factory_extension && object.object->id == ast::ObjectType::TYPE) {
      name += "->create(" + factory_arguments + ")";
    } 
    debug.functionEnd("getName: " + name);
    return name;
  }

  std::string NameConverter::getHierarchyOffsetName(parameters& parm, DatabaseResult& object,
						    int hierarchy_offset) {
    Debug<false> debug("NameConverter::getHierarchyOffsetName");
    std::string name = object.object->name;
    debug.functionStart("getHierarchyOffsetName(name = " + name + ")");
    assert(object.object != NULL);
    debug.debug("Object = " + object.toString());
    name = globalPrefix(parm, object, false, false, hierarchy_offset) + name;
    debug.functionEnd("getHierarchyOffsetName: " + name);
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
  
  std::string NameConverter::getName(std::string& name, ast::ObjectInterface& interface,
                                     ast::ObjectValueContainer& return_type) {
    std::string s = name;
    for (const ast::ObjectInterfaceElement& o : interface.getList()) {
      s += "_" + toLower(o.m_type_name);
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

  std::string NameConverter::getResolvedName(std::string s) {
    return s + "_Resolved";
  }
  
  std::string NameConverter::objectName(ast::ObjectType type, const std::string& name) {
    return objectTypeToString(type) + "_" + name;
  }
  
  std::string NameConverter::objectTypeToString(ast::ObjectType type) {
    std::string type_name = ast::toString(type);
    type_name[0] = toupper(type_name[0]);
    return type_name;
  }
  
  bool NameConverter::isRealNumber(std::string& s) {
    return (s.find(".") != std::string::npos);
  }

  std::string NameConverter::getFactoryInstanceName(std::string s) {
    return "factory_" + s;
  }

  std::string NameConverter::listAppend(std::string& e) {
    return (e.empty() ? "" : ", " + e);
  }

  std::string NameConverter::makeStringPrintable(std::string s) {
    boost::replace_all(s, "\"", "\\\"");
    return s;
  }
  
}
