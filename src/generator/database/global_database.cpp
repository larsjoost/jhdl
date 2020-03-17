#include <cassert>

#include "general.hpp"
#include "global_database.hpp"

namespace generator {
  
  void GlobalDatabase::append(std::shared_ptr<LocalDatabase>& database, std::string& library, std::string& object_name, ast::ObjectType type, ast::Text* source_text) {
    m_debug.functionStart("append(library = " + library + ", object_name = " + object_name + ")", false, __FILE__, __LINE__);
    Object object;
    object.type = type;
    object.database = database;
    object.source_text = source_text;
    assert(library.size() > 0);
    assert(object_name.size() > 0);
    auto l = a_map.find(library);
    if (l != a_map.end()) {
      auto found_object = l->second.find(object_name);
      if (found_object == l->second.end()) {
	l->second[object_name] = object;
      } else {
        m_exceptions.printError(library + "." + object_name + " is already defined", source_text, __FILE__, __LINE__);
        m_exceptions.printError("Defined here:", found_object->second.source_text, __FILE__, __LINE__);
      }
    } else {
      std::unordered_map<std::string, Object> x;
      x[object_name] = object;
      a_map[library] = x;
    }
    m_debug.functionEnd("append");
  }

  bool GlobalDatabase::findObject(std::shared_ptr<LocalDatabase>& object, std::string& library, const std::string& name, ast::ObjectType type) {
    bool found = false;
    auto l = a_map.find(library);
    if (l != a_map.end()) {
      auto x = l->second.find(name);
      if (x != l->second.end()) {
	Object& o = x->second;
	if (type == ast::ObjectType::UNKNOWN || type == o.type) {
	  object = o.database;
	  found = true;
	}
      }
    }
    return found;
  }

  bool GlobalDatabase::findAll(DatabaseResults& results, const std::string& name,
			       std::string& package, std::string& library) {
    m_debug.functionStart("findAll(name = " + name + ", library = " + library + ", package = " + package + ")");
    auto func =
      [&](std::shared_ptr<LocalDatabase>& l, std::string& library, std::string package) {
	std::shared_ptr<std::list<std::string>> hierarchy = std::make_shared<std::list<std::string>>();
	hierarchy->push_back(library);
	hierarchy->push_back(package);
	auto action_callback =
	  [&](DatabaseResult& r) {
	    r.hierarchy = hierarchy;
	    r.local = false;
	  };
	l->findAll(results, name, action_callback);
    };
    bool found = traverseAll(library, package, func);
    m_debug.functionEnd("findAll: found = " + std::to_string(found) + ", size = " + std::to_string(results.size()));
    return found;
  }

  bool GlobalDatabase::setVisible(std::string name, std::string package, std::string library) {
    std::string n = (name == "ALL") ? "" : name;
    auto func =
      [&](std::shared_ptr<LocalDatabase>& l, std::string& library, std::string package) {
	l.get()->setVisible(n);
      };
    return traverseAll(library, package, func);
  }

  void GlobalDatabase::print(std::unordered_map<std::string, Object>& m) {
    for (auto& i : m) {
      std::cout << "[GLOBAL] object = " << i.first << std::endl;
      i.second.database->print();
    }
  }
  
  void GlobalDatabase::print(std::string library) {
    if (library.empty()) {
      for (auto& i : a_map) {
        std::cout << "[GLOBAL] library = " << i.first << std::endl;
        print(i.second);
      }
    } else {
      auto i = a_map.find(library);
      if (i != a_map.end()) {
        std::cout << "[GLOBAL] library = " << i->first << std::endl;
        print(i->second);
      } else {
	std::cerr << "Did not find " + library + " in global database" << std::endl;
      }
             
    }
  }
  
}
  

